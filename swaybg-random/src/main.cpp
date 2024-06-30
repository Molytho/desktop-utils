#include "wayland/display.h"
#include "wayland/output.h"
#include "wayland/registry_manager.h"

#include "event_loop.h"
#include "loop_source/timer.h"
#include "loop_source/wayland.h"
#include "loop_source/signal.h"

#include <iostream>
#include <sys/signalfd.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    using namespace wayland;
    using registry_manager = registry_manager<wl_interface_info<wl_output, 4, 4>>;

    auto display = display_connect();
    registry_manager reg {display};
    auto& manager = reg.get_manager<wl_output>();
    manager.add_added_listener([](const auto& global) {
        std::cout << "Added wl_output global with id " << global->get_id() << std::endl;
    });
    manager.add_removed_listener([](const auto& global) {
        std::cout << "Removed wl_output global with id " << global->get_id() << std::endl;
    });

    event_loop main_loop;
    main_loop.add_item(wayland_source{display}, read_and_dispatch);
    {
        using namespace std::chrono_literals;
        timer timer_source{};
        timer_source.start(1s);

        main_loop.add_item(std::move(timer_source), [](uint64_t) {
            static uint64_t prev = 0;
            uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
            uint64_t diff = now - prev;
            std::cout << "Timer triggered at " << diff << std::endl;
            prev = now;
            return true;
        });
    }
    {
        std::array signals = {SIGTERM, SIGUSR1};
        main_loop.add_item(signal_source{signals}, [&main_loop](signalfd_siginfo& info) {
            if (info.ssi_signo == SIGTERM) {
                main_loop.stop();
            } else if (info.ssi_signo == SIGUSR1) {
                std::cout << "Received SIGUSR1" << std::endl;
            }
        });
    }

    main_loop.run();
}

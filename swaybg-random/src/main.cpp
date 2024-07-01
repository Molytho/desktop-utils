#include "wayland/display.h"
#include "wayland/output.h"
#include "wayland/registry_manager.h"

#include "event_loop.h"
#include "loop_source/timer.h"
#include "loop_source/wayland.h"
#include "loop_source/signal.h"

#include <iostream>
#include <charconv>
#include <sys/signalfd.h>

namespace {
    std::chrono::seconds parse_time_argument(const char *arg) {
        size_t size = strlen(arg);

        uint32_t value {};
        if (std::from_chars(arg, arg + size, value).ec == std::errc{}) {
            return std::chrono::minutes {value};
        } else {
            std::cerr << "Invalid minutes argument" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    std::pair<const char*, std::chrono::seconds> parse_arguments(int argc, char* argv[]) {
        if (argc != 3) {
            std::cerr << "Invalid arguments." << std::endl;
            std::cerr << "Usage: swaybg-random <dir-path> <minutes>" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        return std::make_pair(argv[1], parse_time_argument(argv[2]));
    }

    using registry_manager = wayland::registry_manager<wayland::wl_interface_info<wl_output, 4, 4>>;
}

int main(int argc, char* argv[]) {
    auto [path, seconds] = parse_arguments(argc, argv);

    auto display = wayland::connect();
    registry_manager reg {display};

    auto& output_manager = reg.get_manager<wl_output>();
    output_manager.add_added_listener([](const auto& global) {
        std::cout << "Added wl_output global with id " << global->get_id() << std::endl;
    });
    output_manager.add_removed_listener([](const auto& global) {
        std::cout << "Removed wl_output global with id " << global->get_id() << std::endl;
    });

    event_loop main_loop;
    main_loop.add_item(wayland_source{display}, wayland::read_and_dispatch);
    {
        auto token = main_loop.add_item(timer{}, [](uint64_t) {
            static uint64_t prev = 0;
            uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
            uint64_t diff = now - prev;
            std::cout << "Timer triggered at " << diff << std::endl;
            prev = now;
            return true;
        });
        main_loop[token].first.start(seconds);
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

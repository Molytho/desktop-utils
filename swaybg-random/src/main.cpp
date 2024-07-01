#include "wayland/display.h"
#include "wayland/output.h"
#include "wayland/registry_manager.h"

#include "event_loop.h"
#include "loop_source/timer.h"
#include "loop_source/wayland.h"
#include "loop_source/signal.h"

#include "background_manager.h"
#include "picture_manager.h"

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

    void noop(auto...) {}
    void output_handle_name(void *data,
                            struct wl_output *wl_output,
                            const char *name) {
        auto& bg_manager = *reinterpret_cast<background_manager*>(data);

        //TODO: :(
        uint32_t id = wl_proxy_get_id(reinterpret_cast<wl_proxy*>(wl_output));
        bg_manager.add_output(output{id, name});
    }
    constexpr wl_output_listener output_listener = {
            .geometry = noop,
            .mode = noop,
            .done = noop,
            .scale = noop,
            .name = output_handle_name,
            .description = noop,
    };


    auto setup_timer_source(event_loop& loop, std::chrono::seconds seconds, background_manager& manager) {
        auto token = loop.add_item(timer{}, [&manager](uint64_t) {
            manager.on_timer_expired();
            return true;
        });
        loop[token].first.start(seconds);
        return token;
    }

    void setup_signal_source(event_loop& loop, background_manager& manager, auto timer_token) {
        std::array signals = {SIGTERM, SIGUSR1, SIGCHLD};
        loop.add_item(signal_source{signals}, [&loop, &manager, timer_token](const signalfd_siginfo& info) {
            if (info.ssi_signo == SIGTERM) {
                loop.stop();
            } else if (info.ssi_signo == SIGUSR1) {
                manager.on_timer_expired();
                loop[timer_token].first.reset();
            } else if (info.ssi_signo == SIGCHLD) {
                auto pid = static_cast<pid_t>(info.ssi_pid);
                for (auto& output: manager.outputs()) {
                    if (output.child_pid() == pid) {
                        output.on_child_died(info.ssi_status);
                        break;
                    }
                }
            }
        });
    }
}

int main(int argc, char* argv[]) {
    auto [path, seconds] = parse_arguments(argc, argv);

    auto display = wayland::connect();
    background_manager background_manager { picture_manager{path} };

    registry_manager registry_manager {display};
    auto& output_manager = registry_manager.get_manager<wl_output>();
    output_manager.add_added_listener([&](const auto& output) {
        output->set_listeners(&output_listener, &background_manager);
    });
    output_manager.add_removed_listener([&](const auto& output) {
        background_manager.remove_output(output->get_id());
    });

    event_loop main_loop;
    main_loop.add_item(wayland_source{display}, wayland::read_and_dispatch);
    auto timer_token = setup_timer_source(main_loop, seconds, background_manager);
    setup_signal_source(main_loop, background_manager, timer_token);
    main_loop.run();
}

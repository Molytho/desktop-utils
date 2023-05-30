#include "../include/pictures.h"
#include "../include/output_handler.h"
#include "../include/wayland.h"
#include "../include/timer.h"

#include <iostream>
#include <poll.h>
#include <csignal>
#include <cassert>

#define WAYLAND_FD_INDEX 0
#define TIMER_FD_INDEX 1

bool run = true;

void handle_sigterm(int signo) {
    run = false;
}

void init_signals() {
    struct sigaction sigterm_action;
    sigterm_action.sa_flags = 0;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_handler = handle_sigterm;

    assert (!sigaction(SIGTERM, &sigterm_action, nullptr));
}

int main(int argc, char* argv[]) {
    if (argc > 2 || argc < 2) {
        std::cerr << "Invalid arguments." << std::endl;
        exit(-1);
    }

    Pictures pictures(argv[1]);
    OutputHandler outputHandler(pictures);
    Wayland wayland(outputHandler);
    Timer timer(outputHandler, 1);

    struct pollfd pollfds[] {
            [WAYLAND_FD_INDEX] = {
                    .fd = wayland.get_fd(),
                    .events = POLLIN,
                    .revents = 0,
            },
            [TIMER_FD_INDEX] = {
                    .fd = timer.get_fd(),
                    .events = POLLIN,
                    .revents = 0,
            }
    };

    init_signals();

    wayland.pre_loop();
    while (run) {
        int ret = poll(pollfds, 2, -1);
        if (ret == -1) {
            if (errno == EINTR)
                continue;
            else {
                perror("poll failed");
                exit(EXIT_FAILURE);
            }
        }

        if (pollfds[WAYLAND_FD_INDEX].revents != 0) {
            wayland.handle_ready(pollfds[WAYLAND_FD_INDEX].revents);
        }
        if (pollfds[TIMER_FD_INDEX].revents != 0) {
            timer.handle_action(pollfds[TIMER_FD_INDEX].revents);
        }
    }
    wayland.post_loop();
}
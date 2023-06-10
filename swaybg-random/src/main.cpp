#include "../include/pictures.h"
#include "../include/output_handler.h"
#include "../include/wayland.h"
#include "../include/timer.h"
#include "../include/child_handler.h"

#include <iostream>
#include <poll.h>
#include <csignal>
#include <cassert>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

bool run = true;
OutputHandler* outputHandlerRef = nullptr;

void handle_sigterm(int signo) {
    run = false;
}

void handle_sigusr1(int signo) {
    if (outputHandlerRef)
        outputHandlerRef->next_picture();
}

struct arguments {
    char* directory_path;
    int minutes;
};

void init_signals() {
    struct sigaction sigterm_action;
    sigterm_action.sa_flags = 0;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_handler = handle_sigterm;

    assert (!sigaction(SIGTERM, &sigterm_action, nullptr));


    struct sigaction sigusr1_action;
    sigusr1_action.sa_flags = 0;
    sigemptyset(&sigusr1_action.sa_mask);
    sigusr1_action.sa_handler = handle_sigusr1;

    assert (!sigaction(SIGUSR1, &sigusr1_action, nullptr));
}

arguments parse_arguments(int argc, char* argv[]) {
    if (argc > 3 || argc < 3) {
        std::cerr << "Invalid arguments." << std::endl;
        std::cerr << "Usage: swaybg-random <dir-path> <minutes>" << std::endl;
        exit(-1);
    }

    return { argv[1], std::atoi(argv[2]) };
}

int main(int argc, char* argv[]) {
    auto args = parse_arguments(argc, argv);

    Pictures pictures(args.directory_path);
    OutputHandler outputHandler(pictures);
    Wayland wayland(outputHandler);
    Timer timer(outputHandler, args.minutes);
    child_handler.init();


    outputHandlerRef = &outputHandler;
    init_signals();


    LoopElement* loopElements[] = {
            &wayland,
            &timer
    };

    struct pollfd pollfds[ARRAY_SIZE(loopElements)];
    for (size_t i = 0; i < ARRAY_SIZE(pollfds); i++) {
        pollfds[i] = {
                .fd = loopElements[i]->get_fd(),
                .events = loopElements[i]->get_mask(),
                .revents = 0
        };
    }

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

        for (size_t i = 0; i < ARRAY_SIZE(pollfds); i++) {
            int events = pollfds[i].revents;
            if (events)
                loopElements[i]->handle_ready(events);
        }
    }
    wayland.post_loop();

    outputHandlerRef = nullptr;
}
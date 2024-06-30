#include "../include/timer.h"
#include <sys/timerfd.h>
#include <iostream>
#include <assert.h>
#include <poll.h>

Timer::Timer(OutputHandler& outputHandler, unsigned int minutes) : fd(timerfd_create(CLOCK_BOOTTIME, TFD_CLOEXEC)), outputHandler(outputHandler) {
    assert(fd != -1);

    struct itimerspec timer_settings {
        .it_interval {
            .tv_sec = 60 * minutes,
            .tv_nsec = 0
        },
        .it_value {
            .tv_sec = 60 * minutes,
            .tv_nsec = 0
        }
    };
    timerfd_settime(fd, 0, &timer_settings, nullptr);
}

void Timer::handle_ready(int events) {
    if (events & POLLERR) {
        std::cerr << "Error on timerfd" << std::endl;
        exit(EXIT_FAILURE);
    } else if (events & POLLIN) {
        uint64_t expiration_count;
        read(fd, &expiration_count, sizeof(expiration_count));
        outputHandler.next_picture();
    } else {
        assert(false);
    }
}
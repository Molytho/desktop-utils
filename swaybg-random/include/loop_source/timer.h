#ifndef SWAYBG_RANDOM_TIMER_H
#define SWAYBG_RANDOM_TIMER_H

#include <chrono>
#include <functional>
#include <poll.h>
#include <iostream>

#include "owning_fd.h"

class timer {
    owning_fd m_timerfd;

public:
    timer();

    void start(std::chrono::seconds seconds, bool auto_restart = true);
    void stop();

    // event_source implementation
    const int fd;
    const short mask {POLLIN};

    void pre_sleep() const noexcept { }

    bool post_sleep(short events, const auto& callback) const noexcept {
        if (events & POLLERR) {
            std::cerr << "Error on timerfd" << std::endl;
            exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            uint64_t expiration_count;
            int res = m_timerfd.read(std::as_writable_bytes(std::span{&expiration_count, 1}));
            if (res == -1) {
                perror("Error at timerfd read");
            }
            return callback(expiration_count);
        }
        return true;
    }
};

#endif //SWAYBG_RANDOM_TIMER_H

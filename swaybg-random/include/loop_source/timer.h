#ifndef SWAYBG_RANDOM_TIMER_H
#define SWAYBG_RANDOM_TIMER_H

#include <chrono>
#include <iostream>

#include <poll.h>

#include "event_loop.h"
#include "owning_fd.h"
#include "util.h"

class timer {
    owning_fd m_timerfd;
    itimerspec m_current_spec {};

public:
    using CallbackType = bool(uint64_t);

    timer();

    void start(std::chrono::seconds seconds, bool auto_restart = true);
    void stop();
    void reset();

    [[nodiscard]] constexpr std::chrono::seconds armed_time() const {
        return std::chrono::seconds {m_current_spec.it_interval.tv_sec};
    }

    // event_source implementation
    int fd() const noexcept { return m_timerfd; }

    short mask() const noexcept { return POLLIN; }

    template<util::invokable_with_signature<CallbackType> Callback>
    bool post_sleep(short events, const Callback &callback) const noexcept {
        if (events & POLLERR) {
            std::cerr << "Error on timerfd" << std::endl;
            std::exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            uint64_t expiration_count;
            ssize_t res = m_timerfd.read(expiration_count);
            if (res == -1) {
                perror("Error at timerfd read");
                std::exit(EXIT_FAILURE);
            }
            return util::invoke_or_exit_on_exception(callback, expiration_count);
        }
        return true;
    }
};

static_assert(event_source<timer, timer::CallbackType *>);

#endif //SWAYBG_RANDOM_TIMER_H

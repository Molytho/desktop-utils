#include "include/new/loop_source/timer.h"

#include <sys/timerfd.h>
#include <system_error>

timer::timer() : m_timerfd{timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)}, fd{m_timerfd} {
    if (!m_timerfd) {
        throw std::system_error(errno, std::system_category());
    }
}

void timer::start(std::chrono::seconds seconds, bool auto_restart) {
    struct itimerspec timer_settings {};
    timer_settings.it_value = {
        .tv_sec = seconds.count(),
        .tv_nsec = 0
    };
    if (auto_restart) {
        timer_settings.it_interval = timer_settings.it_value;
    }
    timerfd_settime(fd, 0, &timer_settings, nullptr);
}

void timer::stop() {
    struct itimerspec timer_settings {};
    timerfd_settime(fd, 0, &timer_settings, nullptr);
}
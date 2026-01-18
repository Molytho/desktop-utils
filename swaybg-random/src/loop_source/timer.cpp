#include "loop_source/timer.h"

#include <sys/timerfd.h>
#include <system_error>

timer::timer() : m_timerfd {timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC)} {
    if (!m_timerfd) {
        throw std::system_error(errno, std::system_category());
    }
}

void timer::start(std::chrono::seconds seconds, bool auto_restart) {
    if (seconds.count() == 0) {
        throw std::invalid_argument {"Request to start timer with 0 seconds"};
    }

    m_current_spec          = {};
    m_current_spec.it_value = {.tv_sec = seconds.count(), .tv_nsec = 0};
    if (auto_restart) {
        m_current_spec.it_interval = m_current_spec.it_value;
    }
    timerfd_settime(m_timerfd, 0, &m_current_spec, nullptr);
}

void timer::stop() {
    m_current_spec = {};
    timerfd_settime(m_timerfd, 0, &m_current_spec, nullptr);
}

void timer::reset() {
    timerfd_settime(m_timerfd, 0, &m_current_spec, nullptr);
}
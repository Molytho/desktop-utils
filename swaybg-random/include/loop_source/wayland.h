#ifndef SWAYBG_RANDOM_WAYLAND_H
#define SWAYBG_RANDOM_WAYLAND_H

#include <memory>
#include <optional>
#include <poll.h>
#include <cassert>

#include "include/wayland/display.h"

class wayland_source {
    std::shared_ptr<wayland::display> m_display;
    mutable std::optional<wayland::read_guard> m_guard {};

public:
    explicit wayland_source(std::shared_ptr<wayland::display> display) : m_display{std::move(display)}, fd{m_display->get_fd()} { }

    // event_source implementation
    const int fd;
    const short mask {POLLIN};

    void pre_sleep() const noexcept {
        m_display->flush(); //TODO Lookup correct usage
        while (!(m_guard = m_display->prepare_read())) {
            m_display->dispatch_pending();
            m_display->flush();
        }
    }
    bool post_sleep(short events, const auto& callback) const noexcept {
        assert(m_guard);
        if (events & POLLIN) {
            callback(*m_display, *m_guard);
        }
        m_guard.reset();
        return true;
    }
};

namespace wayland {
    void read_and_dispatch(display& display, read_guard& guard) {
        guard.read();
        display.dispatch_pending();
    }
}

#endif //SWAYBG_RANDOM_WAYLAND_H

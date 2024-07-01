#ifndef SWAYBG_RANDOM_WAYLAND_H
#define SWAYBG_RANDOM_WAYLAND_H

#include <memory>
#include <optional>
#include <poll.h>
#include <cassert>

#include "wayland/display.h"

class wayland_source {
    std::shared_ptr<wayland::display> m_display;
    mutable std::optional<wayland::read_guard> m_guard {};

public:
    explicit wayland_source(std::shared_ptr<wayland::display> display) : m_display{std::move(display)}, fd{m_display->get_fd()} { }

    // event_source implementation
    const int fd;
    const short mask {POLLIN};

    void pre_sleep() const noexcept {
        while (!(m_guard = m_display->prepare_read())) {
            m_display->dispatch_pending();
        }
        m_display->flush();
    }

    template<class Callback>
        requires std::is_invocable_r_v<bool, Callback, wayland::display&, wayland::read_guard&>
    bool post_sleep(short events, const Callback& callback) const noexcept {
        assert(m_guard);
        wayland::read_guard guard = *std::move(m_guard);
        if (events & POLLERR) {
            std::cerr << "Error on wayland fd" << std::endl;
            exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            return callback(*m_display, guard);
        } else {
            return true;
        }
    }
};

namespace wayland {
    constexpr auto read_and_dispatch = [](display& display, read_guard& guard) {
        guard.read();
        display.dispatch_pending();
        return true;
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_H

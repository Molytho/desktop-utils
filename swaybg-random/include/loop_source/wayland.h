#ifndef SWAYBG_RANDOM_WAYLAND_H
#define SWAYBG_RANDOM_WAYLAND_H

#include <cassert>
#include <memory>
#include <optional>
#include <poll.h>

#include "event_loop.h"
#include "util.h"
#include "wayland/display.h"

class wayland_source {
    std::shared_ptr<wayland::display> m_display;
    mutable std::optional<wayland::read_guard> m_guard {};

public:
    using CallbackType = bool(wayland::display &, wayland::read_guard &);

    explicit wayland_source(std::shared_ptr<wayland::display> display) :
            m_display {std::move(display)} {}

    // event_source implementation
    int fd() const noexcept { return m_display->get_fd(); }

    short mask() const noexcept { return POLLIN; }

    void pre_sleep() const noexcept {
        while (!(m_guard = m_display->prepare_read())) {
            m_display->dispatch_pending();
        }
        m_display->flush();
    }

    template<util::invokable_with_signature<CallbackType> Callback>
    bool post_sleep(short events, const Callback &callback) const noexcept {
        assert(m_guard);
        wayland::read_guard guard = *std::move(m_guard);
        if (events & POLLERR) {
            std::cerr << "Error on wayland fd" << std::endl;
            std::exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            return util::invoke_or_exit_on_exception(callback, *m_display, guard);
        } else {
            return true;
        }
    }
};

static_assert(event_source<wayland_source, wayland_source::CallbackType *>);

namespace wayland {
    constexpr auto read_and_dispatch = [](display &display, read_guard &guard) {
        guard.read();
        display.dispatch_pending();
        return true;
    };
} // namespace wayland

#endif //SWAYBG_RANDOM_WAYLAND_H

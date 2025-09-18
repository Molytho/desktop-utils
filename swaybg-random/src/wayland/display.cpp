#include "wayland/display.h"

#include <system_error>

namespace wayland {
    namespace {
        template<int Func(wl_display *)>
        int call_wayland_function(wl_display *display) {
            int res = Func(display);
            if (res == -1) {
                throw std::system_error(errno, std::system_category());
            }
            return res;
        }
    }

    std::shared_ptr<display> connect(const char *name) {
        return std::make_shared<display>(name);
    }

    std::shared_ptr<display> connect(int fd) {
        return std::make_shared<display>(fd);
    }

    display::display(const char *name) : Base {wl_display_connect(name)} {
        if (!m_handle) {
            throw std::system_error(errno, std::system_category());
        }
    }

    display::display(int fd) : Base {wl_display_connect_to_fd(fd)} {
        if (!m_handle) {
            throw std::system_error(errno, std::system_category());
        }
    }

    int display::roundtrip() {
        return call_wayland_function<wl_display_roundtrip>(m_handle.get());
    }

    int display::dispatch() {
        return call_wayland_function<wl_display_dispatch>(m_handle.get());
    }

    int display::dispatch_pending() {
        return call_wayland_function<wl_display_dispatch_pending>(m_handle.get());
    }

    int display::flush() {
        return call_wayland_function<wl_display_flush>(m_handle.get());
    }

    std::optional<read_guard> display::prepare_read() {
        int res = wl_display_prepare_read(m_handle.get());
        if (res == -1) {
            if (errno == EAGAIN) {
                return {};
            } else {
                throw std::system_error(errno, std::system_category());
            }
        }
        return read_guard {*this};
    }

    registry display::create_registry() const {
        return registry {wl_display_get_registry(m_handle.get())};
    }

    read_guard::read_guard(wayland::display &display) noexcept : m_display(&display) {}

    read_guard::read_guard(wayland::read_guard &&other) noexcept : m_display(other.m_display) {
        other.m_display = nullptr;
    }

    read_guard::~read_guard() {
        if (m_display) {
            wl_display_cancel_read(m_display->m_handle.get());
        }
    }

    read_guard &read_guard::operator=(wayland::read_guard &&other) noexcept {
        if (m_display) {
            wl_display_cancel_read(m_display->m_handle.get());
        }
        m_display       = other.m_display;
        other.m_display = nullptr;
        return *this;
    }

    void read_guard::read() {
        wl_display *display = m_display->m_handle.get();
        m_display           = nullptr;
        call_wayland_function<wl_display_read_events>(display);
    }
}
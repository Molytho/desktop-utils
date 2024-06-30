#ifndef SWAYBG_RANDOM_WAYLAND_DISPLAY_H
#define SWAYBG_RANDOM_WAYLAND_DISPLAY_H

#include <system_error>
#include <memory>
#include <optional>

#include "interface.h"
#include "registry.h"
#include "function_object.h"

namespace wayland {
    template<>
    struct resource_delete<wl_display> : public function_object<wl_display_disconnect> {};

    class read_guard;

    class display : interface<wl_display> {
        using Base = interface<wl_display>;
        friend read_guard;

    public:
        explicit display(const char *name = nullptr);
        explicit display(int fd);

        [[nodiscard]] int get_fd() const noexcept {
            return wl_display_get_fd(m_handle.get());
        }

        int roundtrip();
        int dispatch();
        int dispatch_pending();
        int flush();
        [[nodiscard]] std::optional<read_guard> prepare_read();

        [[nodiscard]] registry create_registry() const;
    };

    class read_guard {
        display *m_display;

    public:
        explicit read_guard(display& display) noexcept;
        read_guard(const read_guard&) = delete;
        read_guard(read_guard&&) noexcept;
        ~read_guard();

        read_guard &operator=(read_guard&&) noexcept;

        void read();
    };

    std::shared_ptr<display> display_connect(const char *name = nullptr);
    std::shared_ptr<display> display_connect(int fd);
}

#endif //SWAYBG_RANDOM_WAYLAND_DISPLAY_H

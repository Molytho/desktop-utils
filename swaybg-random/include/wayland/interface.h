#ifndef SWAYBG_RANDOM_WAYLAND_INTERFACE_H
#define SWAYBG_RANDOM_WAYLAND_INTERFACE_H

#include <wayland-client.h>

#include "resource_handle.h"

namespace wayland {
    struct interface_unique_t {};

    struct interface_multiple_t {};

    template<class>
    struct interface_traits;

    template<class T>
    class interface {
        [[nodiscard]] wl_proxy *proxy() const noexcept {
            return reinterpret_cast<wl_proxy *>(m_handle.get());
        }

    protected:
        const resource_handle<T> m_handle;

    public:
        constexpr explicit interface(resource_handle<T> handle) : m_handle {std::move(handle)} {
            if (!m_handle) {
                throw std::logic_error("object was nullptr");
            }
        }

        constexpr explicit interface(T *object) : interface {resource_handle<T> {object}} {}

        void set_user_data(void *data) noexcept { wl_proxy_set_user_data(proxy(), data); }

        [[nodiscard]] void *get_user_data() const noexcept {
            return wl_proxy_get_user_data(proxy());
        }

        [[nodiscard]] wl_display *get_display() const noexcept {
            return wl_proxy_get_display(proxy());
        }

        void set_queue(wl_event_queue *queue) noexcept { wl_proxy_set_queue(proxy(), queue); }

        [[nodiscard]] wl_event_queue *get_queue() const noexcept {
            return wl_proxy_get_queue(proxy());
        }

        [[nodiscard]] uint32_t get_id() const noexcept { return wl_proxy_get_id(proxy()); }

        [[nodiscard]] uint32_t get_version() const noexcept {
            return wl_proxy_get_version(proxy());
        }
    };
} // namespace wayland

#endif //SWAYBG_RANDOM_WAYLAND_INTERFACE_H

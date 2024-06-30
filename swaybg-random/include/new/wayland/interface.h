#ifndef SWAYBG_RANDOM_INTERFACE_H
#define SWAYBG_RANDOM_INTERFACE_H

#include <concepts>
#include <wayland-client.h>

#include "resource_handle.h"

namespace wayland {
    struct interface_unique_t { };
    struct interface_multiple_t { }; // TODO: Englisch

    template<class T>
    concept interface_trait = requires {
        typename T::global;
        typename T::uniqueness;
        requires std::same_as<typename T::uniqueness, interface_unique_t> || std::same_as<typename T::uniqueness, interface_multiple_t>;
        { T::interface() } noexcept -> std::same_as<const wl_interface&>;
    };

    template<class>
    struct interface_traits;

    template<class T>
    class interface {
        [[nodiscard]] wl_proxy *as_proxy() const noexcept {
            return reinterpret_cast<wl_proxy*>(m_handle.get());
        }

    protected:
        const resource_handle<T> m_handle;

    public:
        constexpr explicit interface(T *object) : m_handle{object} {
            if (!m_handle) {
                throw std::logic_error("object was nullptr");
            }
        }
        constexpr explicit interface(resource_handle<T> handle) : m_handle{std::move(handle)} {
            if (!m_handle) {
                throw std::logic_error("object was nullptr");
            }
        }

        void set_user_data(void *data) noexcept {
            wl_proxy_set_user_data(as_proxy(), data);
        }
        [[nodiscard]] void *get_user_data() const noexcept {
            return wl_proxy_get_user_data(as_proxy());
        }

        [[nodiscard]] wl_display *get_display() const noexcept {
            return wl_proxy_get_display(as_proxy());
        }

        void set_queue(wl_event_queue* queue) noexcept {
            wl_proxy_set_queue(as_proxy(), queue);
        }
        [[nodiscard]] wl_event_queue *get_queue() const noexcept {
            return wl_proxy_get_queue(as_proxy());
        }

        [[nodiscard]] uint32_t get_id() const noexcept {
            return wl_proxy_get_id(as_proxy());
        }

        [[nodiscard]] uint32_t get_version() const noexcept {
            return wl_proxy_get_version(as_proxy());
        }
    };
}

#endif //SWAYBG_RANDOM_INTERFACE_H

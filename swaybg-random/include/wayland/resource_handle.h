#ifndef SWAYBG_RANDOM_WAYLAND_RESOURCE_HANDLE_H
#define SWAYBG_RANDOM_WAYLAND_RESOURCE_HANDLE_H

#include <memory>
#include <wayland-client.h>

#include "include/function_object.h"

namespace wayland {
    template<class>
    struct resource_delete;

    template <class T>
    using resource_handle = std::unique_ptr<T, resource_delete<T>>;

    struct proxy_delete {
        constexpr proxy_delete() = default;

        constexpr void operator()(auto* object) const noexcept {
            wl_proxy_destroy(reinterpret_cast<wl_proxy*>(object));
        }
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_RESOURCE_HANDLE_H

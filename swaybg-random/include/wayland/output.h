#ifndef SWAYBG_RANDOM_WAYLAND_OUTPUT_H
#define SWAYBG_RANDOM_WAYLAND_OUTPUT_H

#include "resource_handle.h"
#include "global.h"
#include "concepts.h"

namespace wayland {
    template<>
    struct resource_delete<wl_output> : public proxy_delete {};

    class output : public global<wl_output> {
        using Base = global<wl_output>;

    public:
        using Base::Base;

        void set_listeners(const wl_output_listener *listeners, void *data = nullptr) {
            if (wl_output_add_listener(m_handle.get(), listeners, data) != 0) {
                throw std::runtime_error("Setting listeners failed");
            }
        }
    };

    template<>
    struct interface_traits<wl_output> {
        using object = wl_output;
        using global = output;
        using uniqueness = interface_multiple_t;
        static consteval const wl_interface& interface() noexcept {
            return wl_output_interface;
        }
    };
    static_assert(concepts::interface_trait<interface_traits<wl_output>>);
}

#endif //SWAYBG_RANDOM_WAYLAND_OUTPUT_H

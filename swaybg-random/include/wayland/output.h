#ifndef SWAYBG_RANDOM_WAYLAND_OUTPUT_H
#define SWAYBG_RANDOM_WAYLAND_OUTPUT_H

#include "interface.h"
#include "global.h"
#include "function_object.h"

namespace wayland {
    template<>
    struct resource_delete<wl_output> : public proxy_delete {};

    class output : public global<wl_output> {
        using Base = global<wl_output>;

    public:
        using Base::Base;
    };

    template<>
    struct interface_traits<wl_output> {
        using global = output;
        using uniqueness = interface_multiple_t;
        static consteval const wl_interface& interface() noexcept {
            return wl_output_interface;
        }
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_OUTPUT_H

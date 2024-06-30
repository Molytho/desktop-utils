#ifndef SWAYBG_RANDOM_OUTPUT_H
#define SWAYBG_RANDOM_OUTPUT_H

#include "interface.h"
#include "global.h"

namespace wayland {
    template<>
    struct resource_delete<wl_output> : public utils::function_object<wl_output_destroy> {};

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

#endif //SWAYBG_RANDOM_OUTPUT_H

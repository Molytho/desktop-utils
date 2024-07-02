#ifndef SWAYBG_RANDOM_WAYLAND_CONCEPTS_H
#define SWAYBG_RANDOM_WAYLAND_CONCEPTS_H

#include <cstdint>
#include <concepts>

#include "resource_handle.h"
#include "interface.h"
#include "global.h"

namespace wayland::concepts {
    template<class T, class Global>
    concept global = std::derived_from<Global, global<T>> &&
        std::constructible_from<Global, uint32_t, resource_handle<T>>;

    template<class T>
    concept interface_trait = requires {
        typename T::object;

        typename T::global;
        requires global<typename T::object, typename T::global>;

        typename T::uniqueness;
        requires std::same_as<typename T::uniqueness, interface_unique_t> || std::same_as<typename T::uniqueness, interface_multiple_t>;

        { T::interface() } noexcept -> std::same_as<const wl_interface&>;
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_CONCEPTS_H

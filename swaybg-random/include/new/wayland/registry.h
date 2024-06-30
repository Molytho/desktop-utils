#ifndef SWAYBG_RANDOM_WAYLAND_REGISTRY_H
#define SWAYBG_RANDOM_WAYLAND_REGISTRY_H

#include "interface.h"
#include "new/function_object.h"

namespace wayland {
    template<>
    struct resource_delete<wl_registry> : public proxy_delete {};

    class registry : public interface<wl_registry> {
        using Base = interface<wl_registry>;

    public:
        using Base::Base;

        template<class T, interface_trait traits = interface_traits<T>>
        std::shared_ptr<typename traits::global> bind(uint32_t name, uint32_t version) const {
            const auto& interface = traits::interface();
            resource_handle<T> handle {(T*) wl_registry_bind(m_handle.get(), name, &interface, version)};
            return std::make_shared<typename traits::global>(name, std::move(handle));
        }

        void set_listeners(const wl_registry_listener *listeners, void *data = nullptr) {
            int res = wl_registry_add_listener(m_handle.get(), listeners, data);
            if (res != 0) {
                throw std::runtime_error("Setting listeners failed");
            }
        }
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_REGISTRY_H

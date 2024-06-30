#ifndef SWAYBG_RANDOM_WAYLAND_REGISTRY_MANAGER_H
#define SWAYBG_RANDOM_WAYLAND_REGISTRY_MANAGER_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <span>
#include <tuple>

#include "interface.h"
#include "registry.h"
#include "display.h"

namespace wayland {
    template<class ifs, class = interface_traits<ifs>::uniqueness>
    class interface_manager;
    template<class ifs>
    class interface_manager<ifs, interface_multiple_t> {
        using traits = interface_traits<ifs>;
        using global = typename traits::global;
        using shared_global = std::shared_ptr<global>;

        std::vector<void (*)(const shared_global&)> m_added_listeners;
        std::vector<void (*)(const shared_global&)> m_removed_listeners;
        std::vector<shared_global> m_objects;

    public:
        constexpr interface_manager() = default;
        interface_manager(const interface_manager&) = delete;

        using const_iterator = std::vector<shared_global>::const_iterator;

        std::span<const shared_global> get() const {
            return std::span {m_objects.cbegin(), m_objects.cend()};
        }

        constexpr const_iterator cbegin() const noexcept {
            return m_objects.cbegin();
        }
        constexpr const_iterator cend() const noexcept {
            return m_objects.cend();
        }

        constexpr const_iterator begin() const noexcept {
            return cbegin();
        }
        constexpr const_iterator end() const noexcept {
            return cend();
        }

        constexpr void add_global(shared_global object) {
            m_objects.push_back(std::move(object));
            for (const auto& listener: m_added_listeners) {
                listener(m_objects.back());
            }
        }
        constexpr void remove_global(const_iterator pos) {
            for (const auto& listener : m_removed_listeners) {
                listener(*pos);
            }
            m_objects.erase(pos);
        }

        constexpr void add_added_listener(void (*callback)(const shared_global&)) {
            m_added_listeners.push_back(callback);
        }
        constexpr void add_removed_listener(void (*callback)(const shared_global&)) {
            m_removed_listeners.push_back(callback);
        }

    };
    template<class ifs>
    class interface_manager<ifs, interface_unique_t> {
        static_assert(false);
    };

    template<class T>
    concept interface_info = requires {
        typename T::interface;
        requires interface_trait<interface_traits<typename T::interface>>;
        { T::min_version } -> std::convertible_to<uint32_t>;
        { T::max_version } -> std::convertible_to<uint32_t>;
    };
    template<class T, uint32_t min, uint32_t max = 0>
    struct wl_interface_info {
        using interface = T;
        static constexpr uint32_t min_version = min;
        static constexpr uint32_t max_version = max;
    };

    template<interface_info... interfaces>
    class registry_manager {
        std::shared_ptr<display> m_display;
        registry m_registry {m_display->create_registry()};
        std::tuple<interface_manager<typename interfaces::interface>...> m_managers {};

        template<interface_info ifs, interface_info... remaining>
        void bind_helper(uint32_t id, const char* interface, uint32_t available_version) {
            using type = typename ifs::interface;
            using traits = interface_traits<type>;
            constexpr uint32_t min_version = ifs::min_version;
            constexpr uint32_t max_version = ifs::max_version;

            const wl_interface& wl_interface = traits::interface();
            if (strcmp(interface, wl_interface.name) == 0
                && available_version >= min_version) {
                auto& manager = get_manager<type>();

                uint32_t version;
                if constexpr (max_version == 0) {
                    version = available_version;
                } else {
                    version = std::min(max_version, available_version);
                }

                auto global = m_registry.bind<type>(id, version);
                manager.add_global(std::move(global));
                return;
            }

            if constexpr (sizeof...(remaining) > 0) {
                bind_helper<remaining...>(id, interface, available_version);
            }
        }
        void bind(uint32_t id, const char* interface, uint32_t version) {
            bind_helper<interfaces...>(id, interface, version);
        }

        template<uint32_t index = 0>
        void remove(uint32_t id) {
            if constexpr (index < sizeof...(interfaces)) {
                auto& manager = std::get<index>(m_managers);
                for (auto iter = manager.begin(); iter != manager.end(); ++iter) {
                    auto& object = *iter;
                    if (object->name == id) {
                        object->remove();
                        manager.remove_global(iter);
                        return;
                    }
                }
                remove<index+1>(id);
            }
        }

        static void registry_handle_global(void* data, struct wl_registry*, uint32_t id, const char* interface, uint32_t version) {
            static_cast<registry_manager*>(data)->bind(id, interface, version);
        }
        static void registry_handle_global_remove(void* data, struct wl_registry*, uint32_t id) {
            static_cast<registry_manager*>(data)->remove(id);
        }
        static constexpr wl_registry_listener registry_listener = {
            .global = registry_handle_global,
            .global_remove = registry_handle_global_remove
        };

    public:
        explicit registry_manager(std::shared_ptr<display> display) : m_display{std::move(display)} {
            m_registry.set_listeners(&registry_listener, this);
        }

        template<class T>
        constexpr interface_manager<T>& get_manager()  {
            return std::get<interface_manager<T>>(m_managers);
        }
    };
}

#endif //SWAYBG_RANDOM_WAYLAND_REGISTRY_MANAGER_H

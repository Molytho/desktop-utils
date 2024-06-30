#ifndef SWAYBG_RANDOM_GLOBAL_H
#define SWAYBG_RANDOM_GLOBAL_H

#include "interface.h"

namespace wayland {
	template<class T>
	class global : public interface<T> {
        using Base = interface<T>;

		bool m_removed {false};

	public:
        const uint32_t name;

        global(uint32_t name, resource_handle<T> handle) : Base{std::move(handle)}, name{name} {}
        global(uint32_t name, T *object) : Base{object}, name{name} {}

		constexpr void remove() noexcept {
			m_removed = true;
		}
	};
}

#endif //SWAYBG_RANDOM_GLOBAL_H

#ifndef SWAYBG_RANDOM_WAYLAND_UTILS_H
#define SWAYBG_RANDOM_WAYLAND_UTILS_H

#include <memory>
#include <wayland-client.h>

#include "new/utils.h"

namespace wayland {
	template<class>
	struct resource_delete;

	template <class T>
	using resource_handle = std::unique_ptr<T, resource_delete<T>>;
}

#endif //SWAYBG_RANDOM_WAYLAND_UTILS_H

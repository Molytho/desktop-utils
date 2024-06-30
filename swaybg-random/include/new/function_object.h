#ifndef SWAYBG_RANDOM_FUNCTION_OBJECT_H
#define SWAYBG_RANDOM_FUNCTION_OBJECT_H

#include <utility>
#include <concepts>
#include <functional>

template<auto f>
struct function_object {
	constexpr function_object() = default;

	template<class... Args>
		requires std::invocable<decltype(f), Args...>
	constexpr auto operator()(Args... args) const {
		return std::invoke(f, std::forward<Args>(args)...);
	}
};

#endif //SWAYBG_RANDOM_FUNCTION_OBJECT_H

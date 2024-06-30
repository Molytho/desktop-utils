#ifndef SWAYBG_RANDOM_UTILS_H
#define SWAYBG_RANDOM_UTILS_H

namespace utils {
    template<auto f, class = decltype(f)>
    struct function_object;
    template<auto f, class R, class... Args>
    struct function_object<f, R(*)(Args...)> {
        constexpr function_object() = default;

        constexpr R operator()(Args... args) const {
            return f(std::forward<Args>(args)...);
        }
    };
}

#endif //SWAYBG_RANDOM_UTILS_H

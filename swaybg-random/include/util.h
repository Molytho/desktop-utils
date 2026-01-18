#ifndef SWAYBG_RANDOM_UTIL_H
#define SWAYBG_RANDOM_UTIL_H

#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>

namespace util {
    namespace detail {
        template<class F, class Signature>
        struct invokable_with_signature_helper : std::false_type {};

        template<class F, class R, class... Args>
        struct invokable_with_signature_helper<F, R(Args...)> : std::is_invocable_r<R, F, Args...> {};
    } // namespace detail


    template<class F, class Signature>
    concept invokable_with_signature = detail::invokable_with_signature_helper<F, Signature>::value;

    template<class F, class... Args>
    std::invoke_result_t<F, Args...> invoke_or_exit_on_exception(F &&f, Args &&...args) noexcept try {
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } catch (const std::exception &ex) {
        std::cerr << "Caught exception in invoke_or_exit_on_exception:\n";
        std::cerr << ex.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
} // namespace util

#endif
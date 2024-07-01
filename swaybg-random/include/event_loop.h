#ifndef SWAYBG_RANDOM_EVENT_LOOP_H
#define SWAYBG_RANDOM_EVENT_LOOP_H

#include <vector>
#include <memory>
#include <functional>
#include <span>
#include <poll.h>

#include "owning_fd.h"

// TODO: Maybe an add method instead of fd and mask
template<class T, class Callback>
concept event_source = requires(const T a, short s, const Callback& callback) {
    { a.fd } -> std::convertible_to<int>;
    { a.mask } -> std::convertible_to<short>;
    { a.pre_sleep() } noexcept;
    { a.post_sleep(s, callback) } noexcept -> std::convertible_to<bool>;
} && std::move_constructible<T> && std::move_constructible<Callback>;

namespace detail {
    struct loop_element {
        const int fd;
        const short mask;

        loop_element(int fd, short mask) : fd{fd}, mask{mask} {}
        virtual ~loop_element() = default;

        virtual void pre_sleep() noexcept = 0;
        virtual void post_sleep(short events) noexcept = 0;
    };

    template<class Callback, class Source>
        requires event_source<Source, Callback>
    class event_source_wrapper : public loop_element {
        Source m_source;
        [[no_unique_address]] Callback m_callback;

    public:
        event_source_wrapper(Source source, Callback callback) : loop_element{source.fd, source.mask}, m_source{std::move(source)}, m_callback{std::move(callback)} {}
        ~event_source_wrapper() override = default;

        void pre_sleep() noexcept override {
            m_source.pre_sleep();
        }
        void post_sleep(short events) noexcept override {
            m_source.post_sleep(events, m_callback);
        }

        constexpr Source &get_source() noexcept {
            return m_source;
        }
        constexpr Callback &get_callback() noexcept {
            return m_callback;
        }
    };
}

class event_loop {
    std::vector<std::unique_ptr<detail::loop_element>> m_loop_items {};
    bool m_running {false};
    bool m_should_stop {false};

    std::vector<pollfd> setup_pollfds() noexcept;
    void run_helper(std::span<pollfd> pollfds) noexcept;

public:
    template<class Callback, event_source<Callback> Source>
    using token = detail::event_source_wrapper<Callback, Source>*;

    void run_once();
    void run();
    void stop();

    template<class Callback, event_source<Callback> Source>
    token<Callback, Source> add_item(Source source, Callback callback) {
        using Wrapper = detail::event_source_wrapper<Callback, Source>;
        std::unique_ptr<detail::loop_element> loop_element
            = std::make_unique<Wrapper>(std::move(source), std::move(callback));
        m_loop_items.push_back(std::move(loop_element));
        return static_cast<Wrapper*>(m_loop_items.back().get());
    }

    void remove_item(detail::loop_element *wrapper) {
        erase_if(m_loop_items, [&wrapper](const auto& element) {
            return element.get() == wrapper;
        });
    }

    template<class Callback, event_source<Callback> Source>
    [[nodiscard]] constexpr std::pair<Source&, Callback&> operator[](token<Callback, Source> token) {
        return {token->get_source(), token->get_callback()};
    }
};

#endif //SWAYBG_RANDOM_EVENT_LOOP_H

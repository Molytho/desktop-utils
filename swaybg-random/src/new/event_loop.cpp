#include "new/event_loop.h"

#include <poll.h>
#include <system_error>

std::vector<pollfd> event_loop::setup_pollfds() noexcept {
    std::vector<pollfd> pollfds;
    pollfds.reserve(m_loop_items.size());
    for (const auto& item : m_loop_items) {
        pollfds.push_back({
                                  .fd = item->fd,
                                  .events = item->mask,
                                  .revents = 0
                          });
    }
    return pollfds;
}

void event_loop::run_helper(std::span<pollfd> pollfds) noexcept {
    for (const auto& item: m_loop_items) {
        item->pre_sleep();
    }

    int ret = poll(pollfds.data(), pollfds.size(), -1);
    auto error = errno;

    for (size_t i = 0; i < pollfds.size(); ++i) {
        auto& revents = pollfds.at(i).revents;
        m_loop_items.at(i)->post_sleep(revents);
        revents = 0;
    }

    if (ret == -1 && error != EINTR && error != EAGAIN) {
        throw std::system_error(error, std::system_category());
    }
}

void event_loop::run_once() {
    m_running = true;
    auto pollfds = setup_pollfds();
    run_helper(std::span(pollfds.begin(), pollfds.end()));
    m_running = false;
}

void event_loop::run() {
    m_running = true;
    m_should_stop = false;
    auto pollfds = setup_pollfds();
    auto span = std::span(pollfds.begin(), pollfds.end());
    while (!m_should_stop) {
        run_helper(span);
    }
    m_running = false;
}

void event_loop::stop() {
    m_should_stop = true;
}
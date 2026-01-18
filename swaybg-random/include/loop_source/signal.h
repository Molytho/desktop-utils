#ifndef SWAYBG_RANDOM_SIGNAL_H
#define SWAYBG_RANDOM_SIGNAL_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <span>

#include <poll.h>
#include <sys/signalfd.h>

#include "event_loop.h"
#include "owning_fd.h"
#include "util.h"

class signal_source {
    static constexpr size_t BufferSize = 4;

    owning_fd m_signalfd {};

public:
    using CallbackType = void(const signalfd_siginfo &);

    explicit signal_source(std::span<const int> signals);

    // event_source implementation
    int fd() const noexcept { return m_signalfd; }

    short mask() const noexcept { return POLLIN; };

    template<util::invokable_with_signature<CallbackType> Callback>
    bool post_sleep(short events, const Callback &callback) const noexcept {
        if (events & POLLERR) {
            std::cerr << "Error on timerfd\n";
            std::exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            std::array<signalfd_siginfo, BufferSize> siginfos {};

            //TODO: Maybe factor out into own function
            ssize_t bytes_read;
            while ((bytes_read = m_signalfd.read(std::as_writable_bytes(std::span {siginfos}))) != -1) {
                assert(bytes_read % sizeof(signalfd_siginfo) == 0 && bytes_read > 0);
                size_t count = bytes_read / sizeof(signalfd_siginfo);
                std::for_each_n(siginfos.begin(), count, [&](const signalfd_siginfo &info) {
                    util::invoke_or_exit_on_exception(callback, info);
                });
            }
            if (errno != EAGAIN) {
                perror("Error while reading from signalfd: ");
                std::abort();
            }
        }
        return true;
    }
};

static_assert(event_source<signal_source, signal_source::CallbackType *>);

#endif //SWAYBG_RANDOM_SIGNAL_H

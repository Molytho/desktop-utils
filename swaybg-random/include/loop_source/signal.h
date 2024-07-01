#ifndef SWAYBG_RANDOM_SIGNAL_H
#define SWAYBG_RANDOM_SIGNAL_H

#include <poll.h>
#include <sys/signalfd.h>
#include <cassert>
#include <csignal>
#include <span>
#include <system_error>
#include <algorithm>
#include <iostream>

#include "owning_fd.h"

constexpr size_t signalfd_siginfo_buffer_size = 4;

class signal_source {
    owning_fd m_signalfd {};

public:
    explicit signal_source(std::span<const int> signals);

    // event_source implementation
    const int fd;
    const short mask {POLLIN};

    void pre_sleep() const noexcept { }

    template<class Callback>
        requires std::invocable<Callback, const signalfd_siginfo&>
    bool post_sleep(short events, const Callback& callback) const noexcept {
        if (events & POLLERR) {
            std::cerr << "Error on timerfd" << std::endl;
            exit(EXIT_FAILURE);
        } else if (events & POLLIN) {
            std::array<signalfd_siginfo, signalfd_siginfo_buffer_size> siginfos {};

            //TODO: Maybe factor out into own function
            ssize_t bytes_read;
            while ((bytes_read = m_signalfd.read(std::as_writable_bytes(std::span{siginfos}))) != -1) {
                assert(bytes_read % sizeof(signalfd_siginfo) == 0 && bytes_read > 0);
                size_t count = bytes_read / sizeof(signalfd_siginfo);
                std::for_each_n(siginfos.begin(), count, [&](const signalfd_siginfo& info) {
                    callback(info);
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

#endif //SWAYBG_RANDOM_SIGNAL_H

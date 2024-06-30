#ifndef SWAYBG_RANDOM_SIGNAL_H
#define SWAYBG_RANDOM_SIGNAL_H

#include <csignal>
#include <poll.h>
#include <span>
#include <system_error>
#include <algorithm>
#include <iostream>
#include <sys/signalfd.h>
#include <cassert>

#include "new/owning_fd.h"

constexpr size_t signalfd_siginfo_buffer_size = 4;

class signal_source {
    owning_fd m_signalfd {};

public:
    explicit signal_source(std::span<const int> signals);

    // event_source implementation
    const int fd;
    const short mask {POLLIN};

    void pre_sleep() const noexcept { }
    bool post_sleep(short events, const auto& callback) const noexcept {
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
                std::for_each_n(siginfos.begin(), count, [&](signalfd_siginfo& info) {
                    callback(info);
                });
            }
            if (errno != EAGAIN) {
                throw std::system_error(errno, std::system_category());
            }
        }
        return true;
    }
};

#endif //SWAYBG_RANDOM_SIGNAL_H

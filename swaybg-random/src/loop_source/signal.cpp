#include "loop_source/signal.h"

#include <csignal>

#include <sys/signalfd.h>

namespace {
    sigset_t &operator+=(sigset_t &sigset, int signal) {
        if (sigaddset(&sigset, signal) == -1) {
            throw std::system_error(errno, std::system_category());
        }
        return sigset;
    }

    owning_fd make_signalfd(const sigset_t &sigset) {
        int fd = signalfd(-1, &sigset, SFD_CLOEXEC | SFD_NONBLOCK);
        if (fd == -1) {
            throw std::system_error(errno, std::system_category());
        }
        return owning_fd {fd};
    }
} // namespace

signal_source::signal_source(std::span<const int> signals) {
    sigset_t sigset;
    sigemptyset(&sigset);
    for (const int &signal : signals) {
        sigset += signal;
    }
    [[maybe_unused]] int res = sigprocmask(SIG_BLOCK, &sigset, nullptr);
    assert(res == 0);

    m_signalfd = make_signalfd(sigset);
}
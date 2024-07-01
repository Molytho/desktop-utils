#ifndef SWAYBG_RANDOM_OWNING_FD_H
#define SWAYBG_RANDOM_OWNING_FD_H

#include <unistd.h>
#include <stdexcept>
#include <span>

class owning_fd {
    int m_fd {-1};

public:
    constexpr owning_fd() = default;
    constexpr explicit owning_fd(int fd) : m_fd(fd) {
        if (fd < 0) {
            throw std::invalid_argument("fd is not a valid file descriptor");
        }
    }
    owning_fd(const owning_fd&) = delete;
    owning_fd(owning_fd&& other) noexcept {
        reset(other.m_fd);
        other.m_fd = -1;
    }
    ~owning_fd() {
        reset();
    }

    owning_fd &operator=(owning_fd&& other) noexcept {
        reset(other.m_fd);
        other.m_fd = -1;
        return *this;
    }

    void reset(int fd = -1) noexcept {
        if (m_fd != -1) {
            close(m_fd);
        }
        m_fd = fd;
    }

    operator int() const noexcept {
        return m_fd;
    }

    [[nodiscard]] ssize_t read(auto& variable) const noexcept {
        return read(std::as_writable_bytes(std::span{&variable, 1}));
    }
    [[nodiscard]] ssize_t read(std::span<std::byte> buffer) const noexcept {
        return ::read(m_fd, buffer.data(), buffer.size_bytes());
    }
};

#endif //SWAYBG_RANDOM_OWNING_FD_H

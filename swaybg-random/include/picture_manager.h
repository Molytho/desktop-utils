#ifndef SWAYBG_RANDOM_PICTURE_MANAGER_H
#define SWAYBG_RANDOM_PICTURE_MANAGER_H

#include <vector>

#include "owning_fd.h"

using picture = owning_fd;

class picture_manager {
    size_t m_index {0};
    std::vector<picture> m_pictures;

    static std::vector<picture> build_pictures(const char *path);

public:
    explicit picture_manager(const char *path) : m_pictures{build_pictures(path)} { }
    picture_manager(const picture_manager&) = delete;
    picture_manager(picture_manager&&) = default;

    constexpr void next() {
        ++m_index;
        m_index %= m_pictures.size();
    }
    [[nodiscard]] constexpr const picture &get() const {
        return m_pictures.at(m_index);
    }
};

#endif //SWAYBG_RANDOM_PICTURE_MANAGER_H

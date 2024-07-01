#ifndef SWAYBG_RANDOM_PICTURE_MANAGER_H
#define SWAYBG_RANDOM_PICTURE_MANAGER_H

#include <vector>

#include "owning_fd.h"

using picture = owning_fd;


class picture_manager {
    size_t m_index {0};
    std::vector<picture> m_pictures {};

    void init_pictures(const char *path);

public:
    explicit picture_manager(const char *path);
    picture_manager(const picture_manager&) = delete;
    picture_manager(picture_manager&&) = default;

    void next();
    [[nodiscard]] const picture &get() const;
};

#endif //SWAYBG_RANDOM_PICTURE_MANAGER_H

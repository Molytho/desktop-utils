#include "new/picture_manager.h"

#include <cstring>
#include <algorithm>
#include <random>
#include <array>
#include <system_error>
#include <string>
#include <memory>

#include <dirent.h>
#include <fcntl.h>

#include "new/function_object.h"

namespace {
    constexpr std::array<std::string, 2> endings = {
        ".png",
        ".jpg"
    };

    bool has_correct_ending(const char *str) {
        size_t len = strlen(str);
        return std::any_of(endings.begin(), endings.end(), [&](const auto& ending) {
            return len >= ending.size() && (str + len - ending.size()) == ending;
        });
    }

    using dir_delete = function_object<closedir>;
}

void picture_manager::init_pictures(const char *path) {
    std::unique_ptr<DIR, dir_delete> dir {opendir(path)};
    if (!dir) {
        throw std::system_error(errno, std::system_category());
    }
    int dir_fd = dirfd(dir.get());

    dirent *dirent;
    while ((dirent = readdir(dir.get())) != nullptr) {
        if (dirent->d_type == DT_REG && has_correct_ending(dirent->d_name)) {
            int fd = openat(dir_fd, dirent->d_name, O_RDONLY | O_CLOEXEC);
            if (fd == -1) {
                perror("Error at opening file");
                continue;
            }
            m_pictures.emplace_back(fd);
        }
    }
}

picture_manager::picture_manager(const char* path) {
    init_pictures(path);

    std::random_device rd {};
    std::default_random_engine random {rd()};
    std::shuffle(m_pictures.begin(), m_pictures.end(), random);
}

void picture_manager::next() {
    ++m_index;
    m_index %= m_pictures.size();
}

const picture &picture_manager::get() const {
    return m_pictures.at(m_index);
}
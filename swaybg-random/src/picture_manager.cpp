#include "picture_manager.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <random>
#include <string>
#include <system_error>

#include <dirent.h>
#include <fcntl.h>

#include "function_object.h"

namespace {
    constexpr std::array<std::string, 2> endings = {".png", ".jpg"};

    bool has_correct_ending(const char *str) {
        size_t len = strlen(str);
        return std::any_of(endings.begin(), endings.end(), [&](const auto &ending) {
            return len >= ending.size() && (str + len - ending.size()) == ending;
        });
    }

    using dir_delete = function_object<closedir>;

    template<class Callback>
        requires std::invocable<Callback, const dirent &>
    void for_each_dirent(DIR *directory, Callback callback) {
        dirent *dirent;
        while ((dirent = readdir(directory)) != nullptr) {
            callback(*dirent);
        }
    }

    std::vector<picture> open_pictures_in_directory(const char *path) {
        std::unique_ptr<DIR, dir_delete> dir {opendir(path)};
        if (!dir) {
            throw std::system_error(errno, std::system_category());
        }
        int dir_fd = dirfd(dir.get());

        std::vector<picture> result {};
        for_each_dirent(dir.get(), [&](const dirent &dirent) {
            if (dirent.d_type == DT_REG && has_correct_ending(dirent.d_name)) {
                int fd = openat(dir_fd, dirent.d_name, O_RDONLY | O_CLOEXEC);
                if (fd == -1) {
                    perror("Error at opening file");
                    return;
                }
                result.emplace_back(fd);
            }
        });
        return result;
    }
} // namespace

std::vector<picture> picture_manager::build_pictures(const char *path) {
    std::vector<picture> pictures = open_pictures_in_directory(path);

    std::random_device rd {};
    std::default_random_engine random {rd()};
    std::shuffle(pictures.begin(), pictures.end(), random);

    return pictures;
}
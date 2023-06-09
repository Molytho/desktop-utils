#include "../include/pictures.h"

#include <unordered_set>
#include <cstring>
#include <algorithm>
#include <random>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

const char* endings[] {
        ".png",
        ".jpg"
};

Pictures::Pictures(const char* path) {
    int dirfd = open(path, O_DIRECTORY | O_RDONLY);
    if (dirfd == -1) {
        perror("open picture dir");
        exit(EXIT_FAILURE);
    }

    DIR* dir = fdopendir(dirfd);
    struct dirent *dirent;
    while ((dirent = readdir(dir)) != nullptr) {
        size_t len = strlen(dirent->d_name);

        bool found = false;
        for (const char* ending : endings) {
            assert(strlen(ending) == 4);
            if (strcmp(&dirent->d_name[len - 4], ending) == 0) {
                ;found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }

        if (faccessat(dirfd, dirent->d_name, R_OK, 0) != 0) {
            continue;
        }

        int fd = openat(dirfd, dirent->d_name, O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            perror("Error at opening file");
            continue;
        }
        pictures.push_back(fd);
    }
    closedir(dir);

    std::random_device rd;
    std::default_random_engine random(rd());
    std::shuffle(pictures.begin(), pictures.end(), random);
}

int Pictures::get_next_picture() {
    int fd = pictures.front();
    pictures.pop_front();
    pictures.push_back(fd);

    return fd;
}
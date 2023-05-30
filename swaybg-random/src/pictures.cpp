#include "../include/pictures.h"

#include <filesystem>
#include <string>
#include <unordered_set>
#include <cstring>
#include <algorithm>
#include <random>

std::unordered_set<std::string> endings {
        ".png",
        ".jpg"
};

Pictures::Pictures(const char* path) {
    for (const auto& dir_entry : std::filesystem::directory_iterator(path)) {
        if (!dir_entry.is_regular_file())
            continue;

        if (endings.contains(dir_entry.path().extension())) {
            pictures.push_back(dir_entry.path());
        }
    }

    std::random_device rd;
    std::default_random_engine random(rd());
    std::shuffle(pictures.begin(), pictures.end(), random);
}

char *Pictures::get_next_picture() {
    std::filesystem::path& path = pictures.front();
    char* path_string = strdup(path.c_str());
    pictures.push_back(path);
    pictures.pop_front();

    return path_string;
}
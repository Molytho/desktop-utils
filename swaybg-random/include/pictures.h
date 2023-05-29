#ifndef BUILD_PICTURES_H
#define BUILD_PICTURES_H

#include <filesystem>
#include <deque>

class Pictures {
public:
    Pictures(const char* path);
    char* get_next_picture();

private:
    std::deque<std::filesystem::path> pictures;
};

#endif //BUILD_PICTURES_H

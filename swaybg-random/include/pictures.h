#ifndef BUILD_PICTURES_H
#define BUILD_PICTURES_H

#include <filesystem>
#include <deque>

class Pictures {
public:
    Pictures(const char* path);
    int get_next_picture();

private:
    std::deque<int> pictures; // file descriptors of the files
};

#endif //BUILD_PICTURES_H

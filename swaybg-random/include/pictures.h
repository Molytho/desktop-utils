#ifndef BUILD_PICTURES_H
#define BUILD_PICTURES_H

#include <deque>

class Pictures {
public:
    Pictures(const char* path);

    void next();
    int get() const;

private:
    std::deque<int> pictures; // file descriptors of the files
};

#endif //BUILD_PICTURES_H

#ifndef SWAYBG_RANDOM_CHILD_HANDLER_H
#define SWAYBG_RANDOM_CHILD_HANDLER_H

#include <unordered_map>
#include <unordered_set>

#include "output.h"

class child_handler {
public:
    void init();
    void register_child(int pid, Output& output);
    void shutdown_child(int pid);
    void terminated(int pid);

private:
    std::unordered_map<int, Output&> children;
    std::unordered_set<int> shuting_down;
};

extern class child_handler child_handler;

#endif //SWAYBG_RANDOM_CHILD_HANDLER_H

#ifndef BUILD_TIMER_H
#define BUILD_TIMER_H

#include "output_handler.h"

class Timer {
public:
    Timer(OutputHandler& outputHandler, unsigned int minutes);
    inline int get_fd() {return fd;}
    void handle_action(int events);

private:
    int fd;
    OutputHandler& outputHandler;
};

#endif //BUILD_TIMER_H

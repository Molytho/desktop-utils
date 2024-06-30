#ifndef BUILD_TIMER_H
#define BUILD_TIMER_H

#include "output_handler.h"
#include "loop_element.h"
#include <poll.h>

class Timer : public LoopElement {
public:
    Timer(OutputHandler& outputHandler, unsigned int minutes);
    int get_fd() final { return fd; }
    short get_mask() final { return POLLIN; }
    void handle_ready(int events) final;

private:
    int fd;
    OutputHandler& outputHandler;
};

#endif //BUILD_TIMER_H

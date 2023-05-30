#ifndef BUILD_OUTPUT_HANDLER_H
#define BUILD_OUTPUT_HANDLER_H

#include <wayland-client.h>
#include <deque>
#include "output.h"
#include "pictures.h"

class OutputHandler {
public:
    OutputHandler(Pictures& pictures);
    void add_output(struct wl_output* output);
    void remove_output(struct wl_output* output);
    void next_picture();

private:
    Pictures& pictures;
    std::deque<Output*> outputs;
};

#endif //BUILD_OUTPUT_HANDLER_H

#ifndef BUILD_WAYLAND_H
#define BUILD_WAYLAND_H

#include <wayland-client.h>
#include <functional>
#include <map>
#include "output_handler.h"

class Wayland {
public:
    explicit Wayland(OutputHandler& handler);
    ~Wayland();
    void add_output(uint32_t name);
    void remove_output(uint32_t name);

    void handle_ready(int events);
    inline void pre_loop() { wl_display_prepare_read(display); }
    inline void post_loop() { wl_display_cancel_read(display); }
    inline int get_fd() {return wl_display_get_fd(display);}

private:
    struct wl_display* display;
    struct wl_registry* registry;
    OutputHandler& outputHandler;
    std::map<uint32_t, struct wl_output*> outputs;
};

#endif //BUILD_WAYLAND_H

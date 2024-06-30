#ifndef BUILD_WAYLAND_H
#define BUILD_WAYLAND_H

#include "output_handler.h"
#include "loop_element.h"
#include <wayland-client.h>
#include <functional>
#include <map>
#include <poll.h>

class Wayland : public LoopElement {
public:
    explicit Wayland(OutputHandler& handler);
    ~Wayland();

    void add_output(uint32_t name);
    void on_global_removed(uint32_t name);

    void handle_ready(int events) final;
    inline void pre_loop() { wl_display_prepare_read(display); }
    inline void post_loop() { wl_display_cancel_read(display); }
    int get_fd() final { return wl_display_get_fd(display); }
    short get_mask() final { return POLLIN; }

private:
    struct wl_display* display;
    struct wl_registry* registry;
    OutputHandler& outputHandler;
    std::map<uint32_t, struct wl_output*> outputs;
};

#endif //BUILD_WAYLAND_H

#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H

#include <wayland-client.h>
#include <unistd.h>

class Output {
public:
    Output(struct wl_output* output, char* path);
    ~Output();

    void spawn_swaybg();

    bool operator==(const struct wl_output* output) const;

    const char* output_name;

private:
    void kill_swaybg();

    char* path;
    struct wl_output* output;
    pid_t swaybg_pid;
};

#endif //BUILD_OUTPUT_H

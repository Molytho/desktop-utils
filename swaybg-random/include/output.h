#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H

#include <wayland-client.h>
#include <unistd.h>

class Output {
public:
    Output(struct wl_output* output, char* path);
    ~Output();

    void spawn_swaybg();
    void transition(char* new_picture);

    bool operator==(const struct wl_output* output) const;

    char* output_name;

private:
    char* path;
    struct wl_output* output;
    pid_t swaybg_pid;
};

#endif //BUILD_OUTPUT_H

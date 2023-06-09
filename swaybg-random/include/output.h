#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H

#include <wayland-client.h>
#include <unistd.h>

class Output {
public:
    Output(struct wl_output* output, int picture);
    ~Output();

    void spawn_swaybg();
    void transition(int new_picture);

    bool operator==(const struct wl_output* output) const;

    char* output_name;

private:
    int picture;
    struct wl_output* output;
    pid_t swaybg_pid;
};

#endif //BUILD_OUTPUT_H

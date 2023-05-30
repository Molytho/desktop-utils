#include "../include/output.h"
#include <cassert>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>

void output_handle_geometry(void *data,
                 struct wl_output *wl_output,
                 int32_t x,
                 int32_t y,
                 int32_t physical_width,
                 int32_t physical_height,
                 int32_t subpixel,
                 const char *make,
                 const char *model,
                 int32_t transform) {}

void output_handle_mode(void *data,
             struct wl_output *wl_output,
             uint32_t flags,
             int32_t width,
             int32_t height,
             int32_t refresh) {}

void output_handle_done(void *data,
             struct wl_output *wl_output) {}

void output_handle_scale(void *data,
              struct wl_output *wl_output,
              int32_t factor) {}

void output_handle_name(void *data,
             struct wl_output *wl_output,
             const char *name) {
    auto output = (Output*)data;

    output->output_name = strdup(name);

    output->spawn_swaybg();
}

void output_handle_description(void *data,
                    struct wl_output *wl_output,
                    const char *description) {}

const struct wl_output_listener output_listener = {
        .geometry = output_handle_geometry,
        .mode = output_handle_mode,
        .done = output_handle_done,
        .scale = output_handle_scale,
        .name = output_handle_name,
        .description = output_handle_description,
};

Output::Output(struct wl_output *output, char *path) : output_name(nullptr), path(path), output(output), swaybg_pid(0) {
    wl_output_add_listener(output, &output_listener, this);
}

Output::~Output() {
    if (swaybg_pid > 0) {
        kill(swaybg_pid, SIGTERM);
        waitpid(swaybg_pid, nullptr, 0);
    }
}

void Output::spawn_swaybg() {
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        char* app = "swaybg";
        char* const argv[] = {
                app,
                "-o",
                output_name,
                "-i",
                path,
                nullptr
        };
        execvp(app, argv);
        exit(EXIT_FAILURE);
    } else {
        swaybg_pid = pid;
    }
}

void Output::transition(char *new_picture) {
    path = new_picture;
    pid_t old_pid = swaybg_pid;

    spawn_swaybg();
    sleep(1);

    kill(old_pid, SIGTERM);
    waitpid(old_pid, nullptr, 0);
}

bool Output::operator==(const struct wl_output *output) const {
    return this->output == output;
}
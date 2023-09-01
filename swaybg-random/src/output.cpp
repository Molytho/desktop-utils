#include "../include/output.h"
#include "../include/child_handler.h"
#include <cassert>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <sys/eventfd.h>
#include <cstdio>
#include <iostream>

#define MAX_RETRIES 5

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

Output::Output(struct wl_output *output, int fd) : output_name(nullptr), picture(fd), output(output), swaybg_pid(0), retries(0) {
    wl_output_add_listener(output, &output_listener, this);
}

Output::~Output() {
    if (swaybg_pid > 0) {
        child_handler.shutdown_child(swaybg_pid);
    }
    assert(output_name);
    free(output_name);
    output_name = nullptr;
}

void Output::spawn_swaybg() {
    int event_fd = eventfd(0, EFD_CLOEXEC);

    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        int result = dup2(picture, 0);
        if (result != 0) {
            perror("Error while dupping fd to stdin");
            exit(EXIT_FAILURE);
        }

        const char* app = "swaybg";
        char* const argv[] = {
                strdup(app),
                strdup("-o"),
                output_name,
                strdup("-i"),
                strdup("/dev/stdin"),
                nullptr
        };

        eventfd_t buffer;
        eventfd_read(event_fd, &buffer);
        execvp(app, argv);
        exit(EXIT_FAILURE);
    } else {
        child_handler.register_child(pid, *this);
        swaybg_pid = pid;
        eventfd_write(event_fd, 1);
        close(event_fd);
    }
}

void Output::transition(int new_picture) {
    pid_t old_pid = swaybg_pid;
    picture = new_picture;

    retries = 0;
    spawn_swaybg();
    sleep(1);

    child_handler.shutdown_child(old_pid);
}

bool Output::operator==(const struct wl_output *output) const {
    return this->output == output;
}

void Output::on_swaybg_died(int pid) {
    if (pid != this->swaybg_pid) {
        return;
    }

    if (retries < MAX_RETRIES) {
        retries++;
        spawn_swaybg();
    } else {
        std::cerr << "swaybg failed to often" << std::endl;
    }
}
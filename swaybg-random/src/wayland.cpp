#include "../include/wayland.h"

#include <wayland-client.h>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <poll.h>

void registry_handle_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
    auto* wayland = (Wayland*)data;
    wayland->remove_output(name);
}

void registry_handle_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    auto* wayland = (Wayland*)data;

    if (strcmp(interface, wl_output_interface.name) == 0) {
        assert(version >= 4);
        wayland->add_output(name);
    }
}

const struct wl_registry_listener registry_listener = {
        .global = registry_handle_global,
        .global_remove = registry_handle_global_remove
};

Wayland::Wayland(OutputHandler& outputHandler) : outputHandler(outputHandler) {
    display = wl_display_connect(nullptr);
    if (display == nullptr) {
        perror("Failed to connect to wayland compositor.\n");
        exit(-1);
    }

    registry = wl_display_get_registry(display);
    assert(registry);

    wl_registry_add_listener(registry, &registry_listener, this);

    // Roundtrip twice so that wl_output's get initialized
    wl_display_roundtrip(display);
    wl_display_roundtrip(display);
}

Wayland::~Wayland() {
    wl_registry_destroy(registry);

    for (const auto& output : outputs) {
        outputHandler.remove_output(output.second);
        wl_output_destroy(output.second);
    }

    wl_display_disconnect(display);
}

void Wayland::add_output(uint32_t name) {
    auto* output = (wl_output*)wl_registry_bind(registry, name, &wl_output_interface, 4);
    assert (output);

    outputs.insert({name, output});
    outputHandler.add_output(output);
}

void Wayland::remove_output(uint32_t name) {
    struct wl_output* output = outputs[name];
    outputHandler.remove_output(output);
    outputs.erase(name);

    wl_output_destroy(output);
}

void Wayland::handle_ready(int events) {
    if (events & (POLLERR|POLLHUP)) {
        std::cerr << "Error on wayland fd" << std::endl;
        exit(EXIT_FAILURE);
    } else if (events & POLLIN) {
        wl_display_read_events(display);
        wl_display_dispatch_pending(display);
        wl_display_roundtrip(display);
        wl_display_prepare_read(display);
    } else {
        assert (false);
    }
}
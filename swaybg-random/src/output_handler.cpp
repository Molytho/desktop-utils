#include "../include/output_handler.h"

OutputHandler::OutputHandler(Pictures &pictures) : pictures(pictures) {

}

void OutputHandler::add_output(struct wl_output *output) {
    outputs.emplace_back(output, pictures.get_next_picture());
}

void OutputHandler::remove_output(struct wl_output *output) {
    for (auto it = outputs.cbegin(); it != outputs.cend(); it++) {
        if (*it == output) {
            it = outputs.erase(it);
            break;
        }
    }
}
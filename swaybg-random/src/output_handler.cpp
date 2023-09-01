#include "../include/output_handler.h"

OutputHandler::OutputHandler(Pictures &pictures) : pictures(pictures) {

}

void OutputHandler::add_output(struct wl_output *output) {
    outputs.push_back(new Output(output, pictures.get()));
    pictures.next();
}

void OutputHandler::remove_output(struct wl_output *output) {
    for (auto it = outputs.cbegin(); it != outputs.cend(); it++) {
        auto* element = *it;
        if (*element == output) {
            outputs.erase(it);
            delete element;
            break;
        }
    }
}

void OutputHandler::next_picture() {
    auto next_output = outputs.front();
    next_output->transition(pictures.get());
    pictures.next();
    outputs.push_back(next_output);
    outputs.pop_front();
}
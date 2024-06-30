#include "../include/output_handler.h"

OutputHandler::OutputHandler(Pictures &pictures) : pictures(pictures) {

}

void OutputHandler::add_output(struct wl_output *output) {
    outputs.emplace_back(new Output(output, pictures.get()));
    pictures.next();
}

void OutputHandler::remove_output(struct wl_output *output) {
    for (auto it = outputs.cbegin(); it != outputs.cend(); it++) {
        auto& element = *it;
        if (*element == output) {
            outputs.erase(it);
            break;
        }
    }
}

void OutputHandler::next_picture() {
    auto next_output = std::move(outputs.front());
    outputs.pop_front();
    next_output->transition(pictures.get());
    pictures.next();
    outputs.emplace_back(std::move(next_output));
}
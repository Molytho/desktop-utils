#include "../include/pictures.h"
#include "../include/output_handler.h"
#include "../include/wayland.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 2 || argc < 2) {
        std::cerr << "Invalid arguments." << std::endl;
        exit(-1);
    }

    Pictures pictures(argv[1]);
    OutputHandler outputHandler(pictures);
    Wayland wayland(outputHandler);
}
#include <exception>
#include <iostream>

#include "image.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: PixelVault <image_path>\n";
        return 1;
    }

    try {
        pixelvault::Image image(argv[1]);

        std::cout << "Image loaded successfully\n";
        std::cout << "Width: " << image.width() << '\n';
        std::cout << "Height: " << image.height() << '\n';
        std::cout << "Channels: " << image.channels() << '\n';

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
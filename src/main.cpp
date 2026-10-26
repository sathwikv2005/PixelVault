#include <exception>
#include <iostream>

#include "dct.hpp"
#include "image.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: PixelVault <image_path>\n";
        return 1;
    }

    try {
        pixelvault::Image image(argv[1]);

        cv::Mat coefficients = pixelvault::DCT::forward(image);
        pixelvault::Image reconstructed =
            pixelvault::DCT::inverse(coefficients);

        reconstructed.saveToOutput("reconstructed.png");

        std::cout << "DCT and inverse DCT completed successfully\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
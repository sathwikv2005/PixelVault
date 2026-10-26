#include <exception>
#include <iostream>
#include <string>

#include "decoder.hpp"
#include "encoder.hpp"
#include "image.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << "  PixelVault encode <image> <message>\n"
                  << "  PixelVault decode <image>\n";
        return 1;
    }

    try {
        std::string mode = argv[1];

        if (mode == "encode") {
            if (argc != 4) {
                throw std::invalid_argument(
                    "Encode requires an image and a message");
            }

            pixelvault::Image cover(argv[2]);
            pixelvault::Image stego =
                pixelvault::Encoder::encode(cover, argv[3]);

            stego.saveToOutput("encoded.png");

            std::cout << "Message embedded successfully\n";
        } else if (mode == "decode") {
            if (argc != 3) {
                throw std::invalid_argument("Decode requires an image");
            }

            pixelvault::Image stego(argv[2]);
            std::string message = pixelvault::Decoder::decode(stego);

            std::cout << "Decoded message:\n" << message << '\n';
        } else {
            throw std::invalid_argument("Unknown mode");
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
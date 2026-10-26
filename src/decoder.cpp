#include "decoder.hpp"

#include <cmath>
#include <cstdint>
#include <opencv2/core.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "dct.hpp"

namespace pixelvault {

namespace {

constexpr float QUANTIZATION = 24.0F;
constexpr std::size_t HEADER_BITS = 32;
constexpr int COEFF_ROW = 2;
constexpr int COEFF_COL = 3;

int extractBit(float coefficient) {
    long long index = std::llround(coefficient / QUANTIZATION);
    return static_cast<int>((index % 2 + 2) % 2);
}

std::vector<int> extractBits(const cv::Mat& blue, std::size_t count) {
    std::vector<int> bits;
    bits.reserve(count);

    for (int y = 0; y < blue.rows && bits.size() < count; y += 8) {
        for (int x = 0; x < blue.cols && bits.size() < count; x += 8) {
            float coefficient = blue.at<float>(y + COEFF_ROW, x + COEFF_COL);

            bits.push_back(extractBit(coefficient));
        }
    }

    if (bits.size() != count) {
        throw std::runtime_error("Not enough image capacity to decode message");
    }

    return bits;
}

std::uint32_t readLength(const std::vector<int>& bits) {
    std::uint32_t length = 0;

    for (std::size_t i = 0; i < HEADER_BITS; ++i) {
        length = (length << 1) | static_cast<std::uint32_t>(bits[i]);
    }

    return length;
}

}  // namespace

std::string Decoder::decode(const Image& stego) {
    if (stego.empty()) {
        throw std::invalid_argument("Stego image is empty");
    }

    if (stego.channels() != 3) {
        throw std::invalid_argument("Stego image must have 3 channels");
    }

    cv::Mat coefficients = DCT::forward(stego);

    std::vector<cv::Mat> channels;
    cv::split(coefficients, channels);

    const cv::Mat& blue = channels[0];

    std::size_t capacity = static_cast<std::size_t>(blue.rows / 8) *
                           static_cast<std::size_t>(blue.cols / 8);

    if (capacity < HEADER_BITS) {
        throw std::runtime_error("Image is too small to contain a message");
    }

    std::vector<int> header = extractBits(blue, HEADER_BITS);
    std::uint32_t length = readLength(header);

    if (static_cast<std::uint64_t>(length) > (capacity - HEADER_BITS) / 8) {
        throw std::runtime_error(
            "Invalid message length; image may not contain a valid message");
    }

    std::size_t totalBits = HEADER_BITS + static_cast<std::size_t>(length) * 8;

    std::vector<int> bits = extractBits(blue, totalBits);

    std::string message;
    message.reserve(length);

    for (std::size_t i = HEADER_BITS; i < totalBits; i += 8) {
        unsigned char byte = 0;

        for (int j = 0; j < 8; ++j) {
            byte = static_cast<unsigned char>((byte << 1) | bits[i + j]);
        }

        message.push_back(static_cast<char>(byte));
    }

    return message;
}

}  // namespace pixelvault
#include "encoder.hpp"

#include <cmath>
#include <cstdint>
#include <opencv2/core.hpp>
#include <stdexcept>
#include <vector>

#include "dct.hpp"

namespace pixelvault {

namespace {

constexpr float QUANTIZATION = 24.0F;
constexpr int HEADER_BITS = 32;
constexpr int COEFF_ROW = 2;
constexpr int COEFF_COL = 3;

int parity(long long value) { return static_cast<int>((value % 2 + 2) % 2); }

void embedBit(float& coefficient, int bit) {
    long long index = std::llround(coefficient / QUANTIZATION);

    if (parity(index) != bit) {
        if (coefficient / QUANTIZATION >= index) {
            ++index;
        } else {
            --index;
        }
    }

    coefficient = static_cast<float>(index) * QUANTIZATION;
}

std::vector<int> makeBits(const std::string& message) {
    std::vector<int> bits;
    std::uint32_t length = static_cast<std::uint32_t>(message.size());

    for (int i = 31; i >= 0; --i) {
        bits.push_back((length >> i) & 1U);
    }

    for (unsigned char byte : message) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((byte >> i) & 1U);
        }
    }

    return bits;
}

}  // namespace

Image Encoder::encode(const Image& cover, const std::string& message) {
    if (cover.empty()) {
        throw std::invalid_argument("Cover image is empty");
    }

    if (cover.channels() != 3) {
        throw std::invalid_argument("Cover image must have 3 channels");
    }

    if (message.size() > UINT32_MAX) {
        throw std::invalid_argument("Message is too large");
    }

    cv::Mat coefficients = DCT::forward(cover);

    std::vector<cv::Mat> channels;
    cv::split(coefficients, channels);

    std::vector<int> bits = makeBits(message);

    std::size_t capacity = static_cast<std::size_t>(coefficients.rows / 8) *
                           static_cast<std::size_t>(coefficients.cols / 8);

    if (bits.size() > capacity) {
        throw std::invalid_argument("Message is too large for this image");
    }

    std::size_t bitIndex = 0;
    cv::Mat& blue = channels[0];

    for (int y = 0; y < blue.rows && bitIndex < bits.size(); y += 8) {
        for (int x = 0; x < blue.cols && bitIndex < bits.size(); x += 8) {
            float& coefficient = blue.at<float>(y + COEFF_ROW, x + COEFF_COL);

            embedBit(coefficient, bits[bitIndex++]);
        }
    }

    cv::merge(channels, coefficients);

    return DCT::inverse(coefficients);
}

}  // namespace pixelvault

#include "dct.hpp"

#include <opencv2/core.hpp>
#include <stdexcept>
#include <vector>

namespace pixelvault {

cv::Mat DCT::forward(const Image& image) {
    if (image.empty()) {
        throw std::invalid_argument("Cannot perform DCT on an empty image");
    }

    cv::Mat input;
    image.mat().convertTo(input, CV_32F);

    int paddedWidth = (input.cols + 7) & ~7;
    int paddedHeight = (input.rows + 7) & ~7;

    cv::Mat padded;
    cv::copyMakeBorder(input, padded, 0, paddedHeight - input.rows, 0,
                       paddedWidth - input.cols, cv::BORDER_REPLICATE);

    std::vector<cv::Mat> channels;
    cv::split(padded, channels);

    for (auto& channel : channels) {
        for (int y = 0; y < channel.rows; y += 8) {
            for (int x = 0; x < channel.cols; x += 8) {
                cv::Mat block = channel(cv::Rect(x, y, 8, 8));
                cv::dct(block, block);
            }
        }
    }

    cv::Mat coefficients;
    cv::merge(channels, coefficients);

    return coefficients;
}

Image DCT::inverse(const cv::Mat& coefficients) {
    if (coefficients.empty()) {
        throw std::invalid_argument(
            "Cannot perform inverse DCT on empty coefficients");
    }

    if (coefficients.depth() != CV_32F && coefficients.depth() != CV_64F) {
        throw std::invalid_argument(
            "DCT coefficients must be floating-point values");
    }

    if (coefficients.cols % 8 != 0 || coefficients.rows % 8 != 0) {
        throw std::invalid_argument(
            "DCT coefficient dimensions must be multiples of 8");
    }

    std::vector<cv::Mat> channels;
    cv::split(coefficients, channels);

    for (auto& channel : channels) {
        for (int y = 0; y < channel.rows; y += 8) {
            for (int x = 0; x < channel.cols; x += 8) {
                cv::Mat block = channel(cv::Rect(x, y, 8, 8));
                cv::idct(block, block);
            }
        }
    }

    cv::Mat reconstructed;
    cv::merge(channels, reconstructed);

    cv::Mat output;
    reconstructed.convertTo(output, CV_8U);

    Image image;
    image.mat() = output;

    return image;
}

}  // namespace pixelvault

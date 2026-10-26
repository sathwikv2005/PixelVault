#include "image.hpp"

#include <opencv2/imgcodecs.hpp>
#include <stdexcept>
#include <utility>

namespace pixelvault {

Image::Image(const std::filesystem::path& path) { load(path); }

void Image::load(const std::filesystem::path& path) {
    cv::Mat image = cv::imread(path.string(), cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        throw std::runtime_error("Failed to load image: " + path.string());
    }

    if (image.depth() != CV_8U) {
        throw std::runtime_error("Only 8-bit images are supported");
    }

    data_ = std::move(image);
}

void Image::save(const std::filesystem::path& filename) const {
    if (data_.empty()) {
        throw std::runtime_error("Cannot save an empty image");
    }

    if (filename.empty()) {
        throw std::invalid_argument("Output filename cannot be empty");
    }

    if (!cv::imwrite(filename.string(), data_)) {
        throw std::runtime_error("Failed to save image: " + filename.string());
    }
}

int Image::width() const noexcept { return data_.cols; }

int Image::height() const noexcept { return data_.rows; }

int Image::channels() const noexcept { return data_.channels(); }

bool Image::empty() const noexcept { return data_.empty(); }

const cv::Mat& Image::mat() const noexcept { return data_; }

cv::Mat& Image::mat() noexcept { return data_; }

std::vector<uint8_t> Image::toBytes() const {
    if (data_.empty()) {
        throw std::runtime_error("Cannot convert an empty image");
    }

    if (!data_.isContinuous()) {
        cv::Mat continuous = data_.clone();

        return std::vector<uint8_t>(
            continuous.data,
            continuous.data + continuous.total() * continuous.elemSize());
    }

    return std::vector<uint8_t>(data_.data,
                                data_.data + data_.total() * data_.elemSize());
}

std::vector<uint8_t> Image::toBits() const { return bytesToBits(toBytes()); }

std::vector<uint8_t> Image::bytesToBits(const std::vector<uint8_t>& bytes) {
    std::vector<uint8_t> bits;
    bits.reserve(bytes.size() * 8);

    for (uint8_t byte : bytes) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((byte >> i) & 1);
        }
    }

    return bits;
}

std::vector<uint8_t> Image::bitsToBytes(const std::vector<uint8_t>& bits) {
    if (bits.size() % 8 != 0) {
        throw std::invalid_argument("Bit count must be divisible by 8");
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(bits.size() / 8);

    for (size_t i = 0; i < bits.size(); i += 8) {
        uint8_t byte = 0;

        for (size_t j = 0; j < 8; ++j) {
            if (bits[i + j] > 1) {
                throw std::invalid_argument("Bits must contain only 0 or 1");
            }

            byte = static_cast<uint8_t>((byte << 1) | bits[i + j]);
        }

        bytes.push_back(byte);
    }

    return bytes;
}

Image Image::fromBytes(const std::vector<uint8_t>& bytes, int width, int height,
                       int channels) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Invalid image dimensions");
    }

    int type;

    switch (channels) {
        case 1:
            type = CV_8UC1;
            break;
        case 3:
            type = CV_8UC3;
            break;
        case 4:
            type = CV_8UC4;
            break;
        default:
            throw std::invalid_argument("Channels must be 1, 3, or 4");
    }

    const size_t expectedSize = static_cast<size_t>(width) *
                                static_cast<size_t>(height) *
                                static_cast<size_t>(channels);

    if (bytes.size() != expectedSize) {
        throw std::invalid_argument(
            "Byte count does not match image dimensions");
    }

    Image image;
    image.data_ = cv::Mat(height, width, type);
    std::copy(bytes.begin(), bytes.end(), image.data_.data);

    return image;
}

Image Image::fromBits(const std::vector<uint8_t>& bits, int width, int height,
                      int channels) {
    return fromBytes(bitsToBytes(bits), width, height, channels);
}

void Image::saveToOutput(const std::filesystem::path& filename) const {
    if (filename.empty() || filename.has_parent_path()) {
        throw std::invalid_argument(
            "Provide a filename without directory components");
    }

    const std::filesystem::path outputDirectory("output");

    std::filesystem::create_directories(outputDirectory);
    save(outputDirectory / filename);
}

}  // namespace pixelvault
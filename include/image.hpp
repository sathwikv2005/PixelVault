#pragma once

#include <cstdint>
#include <filesystem>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace pixelvault {

class Image {
   public:
    Image() = default;
    explicit Image(const std::filesystem::path& path);

    void load(const std::filesystem::path& path);
    void save(const std::filesystem::path& filename) const;

    int width() const noexcept;
    int height() const noexcept;
    int channels() const noexcept;
    bool empty() const noexcept;

    const cv::Mat& mat() const noexcept;
    cv::Mat& mat() noexcept;

    std::vector<uint8_t> toBytes() const;
    std::vector<uint8_t> toBits() const;

    static std::vector<uint8_t> bytesToBits(const std::vector<uint8_t>& bytes);

    static std::vector<uint8_t> bitsToBytes(const std::vector<uint8_t>& bits);

    static Image fromBytes(const std::vector<uint8_t>& bytes, int width,
                           int height, int channels);

    static Image fromBits(const std::vector<uint8_t>& bits, int width,
                          int height, int channels);

    void saveToOutput(const std::filesystem::path& filename) const;

   private:
    cv::Mat data_;
};

}  // namespace pixelvault
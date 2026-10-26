
#pragma once

#include <opencv2/core.hpp>

#include "image.hpp"

namespace pixelvault {

class DCT {
   public:
    static cv::Mat forward(const Image& image);
    static Image inverse(const cv::Mat& coefficients);
};

}  // namespace pixelvault

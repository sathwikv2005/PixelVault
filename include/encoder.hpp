#pragma once

#include <string>

#include "image.hpp"

namespace pixelvault {

class Encoder {
   public:
    static Image encode(const Image& cover, const std::string& message);
};

}  // namespace pixelvault
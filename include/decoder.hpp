#pragma once

#include <string>

#include "image.hpp"

namespace pixelvault {

class Decoder {
   public:
    static std::string decode(const Image& stego);
};

}  // namespace pixelvault
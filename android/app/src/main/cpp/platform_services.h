#pragma once

#include <cstdint>

namespace open_crossing {

struct PlatformServices {
    uint64_t frame_number = 0;
    int viewport_width = 0;
    int viewport_height = 0;
};

} // namespace open_crossing

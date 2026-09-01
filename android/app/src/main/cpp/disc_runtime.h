#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "gamecube_disc.h"

namespace open_crossing {

class DiscRuntime {
public:
    bool load_image(const std::vector<std::uint8_t>& image, std::string* error = nullptr);

    bool mounted() const { return mounted_; }
    const GameCubeDisc& disc() const { return disc_; }
    const std::string& last_probe() const { return last_probe_; }

private:
    bool mounted_ = false;
    GameCubeDisc disc_{};
    std::string last_probe_;
};

} // namespace open_crossing

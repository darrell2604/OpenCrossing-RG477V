#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "gamecube_fst.h"

namespace open_crossing {

struct GameCubeDiscInfo {
    std::uint32_t fst_offset = 0;
    std::uint32_t fst_size = 0;
    std::string game_code;
    std::string maker_code;
};

class GameCubeDisc {
public:
    bool open(const std::vector<std::uint8_t>& image, std::string* error = nullptr);

    const GameCubeDiscInfo& info() const { return info_; }
    const GameCubeFst& fst() const { return fst_; }
    const std::vector<std::uint8_t>& image() const { return image_; }

    const GameCubeFstEntry* find(std::string_view path) const { return fst_.find(path); }
    bool read_file(std::string_view path,
                   std::vector<std::uint8_t>* output,
                   std::string* error = nullptr) const;

private:
    std::vector<std::uint8_t> image_;
    GameCubeDiscInfo info_{};
    GameCubeFst fst_{};
};

} // namespace open_crossing

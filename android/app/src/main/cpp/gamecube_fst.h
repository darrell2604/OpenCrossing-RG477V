#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace open_crossing {

struct GameCubeFstEntry {
    std::string path;
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
    bool directory = false;
};

class GameCubeFst {
public:
    bool parse(const std::vector<std::uint8_t>& fst_bytes, std::string* error = nullptr);

    const std::vector<GameCubeFstEntry>& entries() const { return entries_; }
    const GameCubeFstEntry* find(std::string_view path) const;
    bool read_file(std::string_view path,
                   const std::vector<std::uint8_t>& disc_image,
                   std::vector<std::uint8_t>* output,
                   std::string* error = nullptr) const;

private:
    std::vector<GameCubeFstEntry> entries_;
};

} // namespace open_crossing

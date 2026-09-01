#include "disc_runtime.h"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace open_crossing {
namespace {

std::string printable_prefix(const std::vector<std::uint8_t>& data, std::size_t count) {
    std::ostringstream out;
    const std::size_t n = (data.size() < count) ? data.size() : count;
    for (std::size_t i = 0; i < n; ++i) {
        const unsigned char c = data[i];
        if (std::isprint(c) != 0) out << c;
        else if (c == '\r') out << "\\r";
        else if (c == '\n') out << "\\n";
        else out << '.';
    }
    return out.str();
}

} // namespace

bool DiscRuntime::load_image(const std::vector<std::uint8_t>& image, std::string* error) {
    mounted_ = false;
    last_probe_.clear();

    std::string open_error;
    if (!disc_.open(image, &open_error)) {
        if (error) *error = open_error;
        return false;
    }

    const GameCubeDiscInfo& info = disc_.info();
    std::vector<std::uint8_t> static_map;
    std::string read_error;
    const bool read_ok = disc_.read_file("/static.map", &static_map, &read_error);

    std::ostringstream probe;
    probe << "GameCube " << info.game_code
          << " maker=" << info.maker_code
          << " fst_offset=0x" << std::hex << info.fst_offset
          << " fst_size=" << std::dec << info.fst_size
          << " entries=" << disc_.fst().entries().size();

    if (read_ok) {
        probe << " /static.map=" << static_map.size()
              << " bytes prefix=\"" << printable_prefix(static_map, 32) << "\"";
    } else {
        probe << " /static.map read failed: " << read_error;
    }

    last_probe_ = probe.str();
    mounted_ = true;
    if (error) error->clear();
    return true;
}

} // namespace open_crossing

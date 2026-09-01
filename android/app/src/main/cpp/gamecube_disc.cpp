#include "gamecube_disc.h"

namespace open_crossing {
namespace {

constexpr std::size_t kFstOffsetField = 0x424;
constexpr std::size_t kFstSizeField = 0x428;
constexpr std::size_t kGameCodeOffset = 0x0;
constexpr std::size_t kGameCodeLength = 4;
constexpr std::size_t kMakerCodeOffset = 0x4;
constexpr std::size_t kMakerCodeLength = 2;
constexpr std::size_t kDiscHeaderMinimum = 0x42C;

std::uint32_t read_be32(const std::uint8_t* p) {
    return (static_cast<std::uint32_t>(p[0]) << 24) |
           (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) << 8) |
           static_cast<std::uint32_t>(p[3]);
}

void set_error(std::string* error, const char* message) {
    if (error) *error = message;
}

} // namespace

bool GameCubeDisc::open(const std::vector<std::uint8_t>& image, std::string* error) {
    image_.clear();
    info_ = {};
    fst_ = {};

    if (image.size() < kDiscHeaderMinimum) {
        set_error(error, "Disc image is too small for the GameCube filesystem header");
        return false;
    }

    info_.game_code.assign(reinterpret_cast<const char*>(image.data() + kGameCodeOffset), kGameCodeLength);
    info_.maker_code.assign(reinterpret_cast<const char*>(image.data() + kMakerCodeOffset), kMakerCodeLength);

    // GameCube disc header stores the FST offset directly in bytes.
    // The supplied Animal Crossing image records 0x000FE500 here.
    const std::uint32_t fst_offset = read_be32(image.data() + kFstOffsetField);
    const std::uint32_t fst_size = read_be32(image.data() + kFstSizeField);
    const std::uint64_t fst_end = static_cast<std::uint64_t>(fst_offset) + fst_size;

    if (fst_offset > image.size() || fst_end > image.size()) {
        set_error(error, "GameCube FST range is outside the disc image");
        return false;
    }

    info_.fst_offset = fst_offset;
    info_.fst_size = fst_size;
    const auto begin = image.begin() + static_cast<std::size_t>(fst_offset);
    const auto end = begin + static_cast<std::size_t>(fst_size);
    std::vector<std::uint8_t> fst_bytes(begin, end);
    if (!fst_.parse(fst_bytes, error)) return false;

    image_ = image;
    return true;
}

bool GameCubeDisc::read_file(std::string_view path,
                             std::vector<std::uint8_t>* output,
                             std::string* error) const {
    return fst_.read_file(path, image_, output, error);
}

} // namespace open_crossing

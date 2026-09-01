#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace open_crossing {

struct IsoFileEntry {
    std::string name;
    std::uint32_t extent_sector = 0;
    std::uint32_t size = 0;
    bool directory = false;
};

class Iso9660Reader {
public:
    static constexpr std::size_t kSectorSize = 2048;

    bool open(const std::vector<std::uint8_t>& image);
    bool valid() const { return valid_; }
    std::uint32_t sector_count() const { return sector_count_; }

    bool list_directory(const std::string& path, std::vector<IsoFileEntry>& entries) const;
    bool find(const std::string& path, IsoFileEntry& entry) const;
    bool read_file(const IsoFileEntry& entry, std::vector<std::uint8_t>& out) const;

private:
    bool read_sector(std::uint32_t sector, const std::uint8_t*& data) const;
    bool read_directory(std::uint32_t extent, std::uint32_t size,
                        std::vector<IsoFileEntry>& entries) const;
    static std::string normalise_path(const std::string& path);
    static std::string normalise_name(const std::string& name);

    const std::vector<std::uint8_t>* image_ = nullptr;
    std::uint32_t sector_count_ = 0;
    std::uint32_t root_extent_ = 0;
    std::uint32_t root_size_ = 0;
    bool valid_ = false;
};

} // namespace open_crossing

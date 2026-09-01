#include "iso9660_loader.h"

#include <algorithm>
#include <cctype>
#include <cstring>

namespace open_crossing {
namespace {
std::uint16_t read_le16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0] | (static_cast<std::uint16_t>(p[1]) << 8));
}

std::uint32_t read_le32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}
}

std::string Iso9660Reader::normalise_name(const std::string& name) {
    const std::size_t version = name.find(';');
    std::string result = version == std::string::npos ? name : name.substr(0, version);
    while (!result.empty() && result.back() == '.') result.pop_back();
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return result;
}

std::string Iso9660Reader::normalise_path(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    while (!result.empty() && result.front() == '/') result.erase(result.begin());
    while (!result.empty() && result.back() == '/') result.pop_back();
    std::string out;
    std::size_t start = 0;
    while (start < result.size()) {
        const std::size_t end = result.find('/', start);
        const std::string part = normalise_name(result.substr(start, end - start));
        if (!part.empty() && part != ".") {
            if (!out.empty()) out.push_back('/');
            out += part;
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return out;
}

bool Iso9660Reader::open(const std::vector<std::uint8_t>& image) {
    image_ = nullptr;
    sector_count_ = 0;
    root_extent_ = 0;
    root_size_ = 0;
    valid_ = false;

    if (image.size() < 17 * kSectorSize) return false;
    if (image.size() % kSectorSize != 0) return false;

    for (std::uint32_t sector = 16; sector < image.size() / kSectorSize; ++sector) {
        const std::uint8_t* p = image.data() + static_cast<std::size_t>(sector) * kSectorSize;
        if (std::memcmp(p + 1, "CD001", 5) != 0 || p[6] != 1) continue;
        if (p[0] == 1) {
            if (p[128] != 0x00 && read_le16(p + 128) != kSectorSize) return false;
            const std::uint8_t* root = p + 156;
            const std::uint8_t length = root[0];
            if (length < 34 || root + length > p + kSectorSize) return false;
            root_extent_ = read_le32(root + 2);
            root_size_ = read_le32(root + 10);
            sector_count_ = static_cast<std::uint32_t>(image.size() / kSectorSize);
            image_ = &image;
            valid_ = root_extent_ < sector_count_ && root_size_ <= image.size() -
                static_cast<std::size_t>(root_extent_) * kSectorSize;
            return valid_;
        }
        if (p[0] == 255) break;
    }
    return false;
}

bool Iso9660Reader::read_sector(std::uint32_t sector, const std::uint8_t*& data) const {
    if (!valid_ || !image_ || sector >= sector_count_) return false;
    data = image_->data() + static_cast<std::size_t>(sector) * kSectorSize;
    return true;
}

bool Iso9660Reader::read_directory(std::uint32_t extent, std::uint32_t size,
                                    std::vector<IsoFileEntry>& entries) const {
    if (!valid_ || !image_) return false;
    const std::size_t offset = static_cast<std::size_t>(extent) * kSectorSize;
    if (offset > image_->size() || size > image_->size() - offset) return false;

    std::size_t cursor = 0;
    const std::uint8_t* data = image_->data() + offset;
    while (cursor < size) {
        const std::uint8_t length = data[cursor];
        if (length == 0) {
            cursor = ((cursor / kSectorSize) + 1) * kSectorSize;
            continue;
        }
        if (length < 34 || cursor + length > size) return false;
        const std::uint8_t* record = data + cursor;
        const std::uint8_t name_length = record[32];
        if (33u + name_length > length) return false;
        std::string name(reinterpret_cast<const char*>(record + 33), name_length);
        const bool directory = (record[25] & 0x02u) != 0;
        if (name_length != 1 || (static_cast<unsigned char>(name[0]) != 0 &&
                                 static_cast<unsigned char>(name[0]) != 1)) {
            entries.push_back({normalise_name(name), read_le32(record + 2), read_le32(record + 10), directory});
        }
        cursor += length;
    }
    return true;
}

bool Iso9660Reader::list_directory(const std::string& path,
                                   std::vector<IsoFileEntry>& entries) const {
    entries.clear();
    const std::string wanted = normalise_path(path);
    if (wanted.empty()) return read_directory(root_extent_, root_size_, entries);

    IsoFileEntry current{ {}, root_extent_, root_size_, true };
    std::size_t start = 0;
    while (start < wanted.size()) {
        const std::size_t end = wanted.find('/', start);
        const std::string part = wanted.substr(start, end - start);
        std::vector<IsoFileEntry> children;
        if (!read_directory(current.extent_sector, current.size, children)) return false;
        auto it = std::find_if(children.begin(), children.end(), [&](const IsoFileEntry& entry) {
            return entry.directory && entry.name == part;
        });
        if (it == children.end()) return false;
        current = *it;
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return current.directory && read_directory(current.extent_sector, current.size, entries);
}

bool Iso9660Reader::find(const std::string& path, IsoFileEntry& entry) const {
    entry = {};
    const std::string wanted = normalise_path(path);
    if (wanted.empty()) {
        entry = { "/", root_extent_, root_size_, true };
        return true;
    }

    std::size_t start = 0;
    IsoFileEntry current{ "/", root_extent_, root_size_, true };
    while (start < wanted.size()) {
        const std::size_t end = wanted.find('/', start);
        const std::string part = wanted.substr(start, end - start);
        std::vector<IsoFileEntry> children;
        if (!read_directory(current.extent_sector, current.size, children)) return false;
        auto it = std::find_if(children.begin(), children.end(), [&](const IsoFileEntry& candidate) {
            return candidate.name == part;
        });
        if (it == children.end()) return false;
        current = *it;
        if (end == std::string::npos) {
            entry = current;
            return true;
        }
        if (!current.directory) return false;
        start = end + 1;
    }
    return false;
}

bool Iso9660Reader::read_file(const IsoFileEntry& entry, std::vector<std::uint8_t>& out) const {
    out.clear();
    if (!valid_ || entry.directory || !image_) return false;
    const std::size_t offset = static_cast<std::size_t>(entry.extent_sector) * kSectorSize;
    if (entry.extent_sector >= sector_count_ || entry.size > image_->size() - offset) return false;
    out.resize(entry.size);
    if (!out.empty()) std::memcpy(out.data(), image_->data() + offset, out.size());
    return true;
}

} // namespace open_crossing

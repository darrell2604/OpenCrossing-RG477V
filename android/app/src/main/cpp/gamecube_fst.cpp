#include "gamecube_fst.h"

#include <algorithm>
#include <cstring>

namespace open_crossing {
namespace {

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

bool GameCubeFst::parse(const std::vector<std::uint8_t>& fst_bytes, std::string* error) {
    entries_.clear();

    if (fst_bytes.size() < 12) {
        set_error(error, "FST is smaller than one node");
        return false;
    }

    const std::uint32_t node_count = read_be32(fst_bytes.data() + 8);
    if (node_count == 0) {
        set_error(error, "FST contains no nodes");
        return false;
    }

    constexpr std::size_t kNodeSize = 12;
    const std::size_t node_bytes = static_cast<std::size_t>(node_count) * kNodeSize;
    if (node_count > (std::numeric_limits<std::size_t>::max() / kNodeSize) ||
        node_bytes > fst_bytes.size()) {
        set_error(error, "FST node table exceeds supplied data");
        return false;
    }

    const std::size_t string_table_offset = node_bytes;
    if (string_table_offset >= fst_bytes.size()) {
        set_error(error, "FST has no string table");
        return false;
    }

    struct DirectoryFrame {
        std::string path;
        std::uint32_t end_index;
    };

    std::vector<DirectoryFrame> stack;
    stack.push_back({"", node_count});

    entries_.reserve(node_count);
    entries_.push_back({"/", 0, 0, true});

    for (std::uint32_t index = 1; index < node_count; ++index) {
        while (stack.size() > 1 && index >= stack.back().end_index) {
            stack.pop_back();
        }

        const std::size_t node_offset = static_cast<std::size_t>(index) * kNodeSize;
        const std::uint32_t type_name = read_be32(fst_bytes.data() + node_offset);
        const std::uint32_t value = read_be32(fst_bytes.data() + node_offset + 4);
        const std::uint32_t size_or_end = read_be32(fst_bytes.data() + node_offset + 8);

        const bool directory = (type_name & 0x01000000u) != 0;
        const std::uint32_t name_offset = type_name & 0x00FFFFFFu;
        const std::size_t name_pos = string_table_offset + name_offset;
        if (name_pos >= fst_bytes.size()) {
            set_error(error, "FST entry name offset is outside the string table");
            entries_.clear();
            return false;
        }

        const char* name = reinterpret_cast<const char*>(fst_bytes.data() + name_pos);
        const std::size_t max_name = fst_bytes.size() - name_pos;
        if (std::memchr(name, '\0', max_name) == nullptr) {
            set_error(error, "FST entry name is not NUL terminated");
            entries_.clear();
            return false;
        }

        std::string path = stack.back().path;
        if (path.empty()) {
            path = "/" + std::string(name);
        } else {
            path += "/" + std::string(name);
        }

        if (directory) {
            if (size_or_end <= index || size_or_end > node_count) {
                set_error(error, "FST directory end index is invalid");
                entries_.clear();
                return false;
            }
            entries_.push_back({path, 0, 0, true});
            stack.push_back({path, size_or_end});
        } else {
            entries_.push_back({path, value, size_or_end, false});
        }
    }

    return true;
}

const GameCubeFstEntry* GameCubeFst::find(std::string_view path) const {
    for (const auto& entry : entries_) {
        if (entry.path == path) return &entry;
    }
    return nullptr;
}

bool GameCubeFst::read_file(std::string_view path,
                            const std::vector<std::uint8_t>& disc_image,
                            std::vector<std::uint8_t>* output,
                            std::string* error) const {
    if (!output) {
        set_error(error, "Output buffer is null");
        return false;
    }
    output->clear();

    const GameCubeFstEntry* entry = find(path);
    if (!entry) {
        set_error(error, "FST path not found");
        return false;
    }
    if (entry->directory) {
        set_error(error, "FST path refers to a directory");
        return false;
    }

    const std::uint64_t end = static_cast<std::uint64_t>(entry->offset) + entry->size;
    if (end > disc_image.size()) {
        set_error(error, "FST file extends beyond disc image");
        return false;
    }

    output->assign(disc_image.begin() + entry->offset,
                   disc_image.begin() + static_cast<std::size_t>(end));
    return true;
}

} // namespace open_crossing

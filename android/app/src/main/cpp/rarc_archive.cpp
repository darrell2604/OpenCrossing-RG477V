#include "rarc_archive.h"

#include <cstring>
#include <sstream>

namespace open_crossing {
namespace {

std::uint16_t be16(const std::vector<std::uint8_t>& d, std::size_t o) {
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(d[o]) << 8) | d[o + 1]);
}

std::uint32_t be32(const std::vector<std::uint8_t>& d, std::size_t o) {
    return (static_cast<std::uint32_t>(d[o]) << 24) |
           (static_cast<std::uint32_t>(d[o + 1]) << 16) |
           (static_cast<std::uint32_t>(d[o + 2]) << 8) |
           static_cast<std::uint32_t>(d[o + 3]);
}

bool range_ok(std::size_t size, std::size_t offset, std::size_t length) {
    return offset <= size && length <= size - offset;
}

} // namespace

bool RarcArchive::open(const std::vector<std::uint8_t>& data, std::string* error) {
    data_ = data;
    root_entries_.clear();
    node_count_ = file_entry_count_ = 0;
    data_offset_ = string_table_offset_ = string_table_size_ = 0;
    node_offset_ = file_entry_offset_ = 0;

    if (data_.size() < 0x40 || std::memcmp(data_.data(), "RARC", 4) != 0) {
        if (error) *error = "RARC header missing";
        return false;
    }

    const std::uint32_t archive_size = be32(data_, 0x04);
    if (archive_size > data_.size() || be32(data_, 0x08) != 0x20) {
        if (error) *error = "Invalid RARC header";
        return false;
    }

    data_offset_ = 0x20 + be32(data_, 0x0c);
    if (!range_ok(data_.size(), data_offset_, be32(data_, 0x10))) {
        if (error) *error = "RARC file-data range is invalid";
        return false;
    }

    node_count_ = be32(data_, 0x20);
    node_offset_ = 0x20 + be32(data_, 0x24);
    file_entry_count_ = be32(data_, 0x28);
    file_entry_offset_ = 0x20 + be32(data_, 0x2c);
    string_table_size_ = be32(data_, 0x30);
    string_table_offset_ = 0x20 + be32(data_, 0x34);

    if (node_count_ == 0 || node_count_ > 0x10000 ||
        file_entry_count_ > 0x100000 ||
        !range_ok(data_.size(), node_offset_, static_cast<std::size_t>(node_count_) * 0x10) ||
        !range_ok(data_.size(), file_entry_offset_, static_cast<std::size_t>(file_entry_count_) * 0x14) ||
        !range_ok(data_.size(), string_table_offset_, string_table_size_)) {
        if (error) *error = "RARC table range is invalid";
        return false;
    }

    return read_node_entries(0, &root_entries_, error);
}

bool RarcArchive::read_string(std::uint32_t offset, std::string* output) const {
    if (offset >= string_table_size_) return false;
    const std::size_t start = string_table_offset_ + offset;
    const std::size_t end = string_table_offset_ + string_table_size_;
    std::size_t p = start;
    while (p < end && data_[p] != 0) ++p;
    if (p == end) return false;
    output->assign(reinterpret_cast<const char*>(data_.data() + start), p - start);
    return true;
}

bool RarcArchive::read_node_entries(std::uint32_t node_index,
                                    std::vector<RarcEntry>* output,
                                    std::string* error) const {
    if (node_index >= node_count_) {
        if (error) *error = "RARC node index out of range";
        return false;
    }

    const std::size_t node = node_offset_ + static_cast<std::size_t>(node_index) * 0x10;
    const std::uint32_t name_offset = be32(data_, node + 4);
    const std::uint16_t count = be16(data_, node + 0x0a);
    const std::uint32_t first = be32(data_, node + 0x0c);

    if (static_cast<std::uint64_t>(first) + count > file_entry_count_) {
        if (error) *error = "RARC node entry range is invalid";
        return false;
    }

    (void)name_offset;
    output->clear();
    output->reserve(count);

    for (std::uint32_t i = 0; i < count; ++i) {
        const std::size_t entry = file_entry_offset_ + static_cast<std::size_t>(first + i) * 0x14;
        RarcEntry item{};
        item.id = be16(data_, entry);
        item.name_hash = be16(data_, entry + 2);
        item.unknown = be16(data_, entry + 4);
        item.name_offset = be16(data_, entry + 6);
        item.data_offset = be32(data_, entry + 8);
        item.data_size = be32(data_, entry + 0x0c);
        item.directory = (item.id == 0xffff);
        item.directory_index = item.directory ? item.data_offset : 0;

        if (!read_string(item.name_offset, &item.name)) {
            if (error) *error = "RARC entry has an invalid name offset";
            return false;
        }

        if (!item.directory &&
            !range_ok(data_.size(), data_offset_ + item.data_offset, item.data_size)) {
            if (error) *error = "RARC file entry data range is invalid";
            return false;
        }

        output->push_back(std::move(item));
    }

    return true;
}

} // namespace open_crossing

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace open_crossing {

struct RarcEntry {
    std::uint16_t id = 0;
    std::uint16_t name_hash = 0;
    std::uint16_t unknown = 0;
    std::uint16_t name_offset = 0;
    std::uint32_t data_offset = 0;
    std::uint32_t data_size = 0;
    bool directory = false;
    std::uint32_t directory_index = 0;
    std::string name;
};

class RarcArchive {
public:
    bool open(const std::vector<std::uint8_t>& data, std::string* error = nullptr);

    const std::vector<RarcEntry>& root_entries() const { return root_entries_; }
    const std::vector<std::uint8_t>& data() const { return data_; }
    std::uint32_t node_count() const { return node_count_; }
    std::uint32_t file_entry_count() const { return file_entry_count_; }

private:
    bool read_string(std::uint32_t offset, std::string* output) const;
    bool read_node_entries(std::uint32_t node_index,
                           std::vector<RarcEntry>* output,
                           std::string* error) const;

    std::vector<std::uint8_t> data_;
    std::vector<RarcEntry> root_entries_;
    std::uint32_t node_count_ = 0;
    std::uint32_t file_entry_count_ = 0;
    std::uint32_t data_offset_ = 0;
    std::uint32_t string_table_offset_ = 0;
    std::uint32_t string_table_size_ = 0;
    std::uint32_t node_offset_ = 0;
    std::uint32_t file_entry_offset_ = 0;
};

} // namespace open_crossing

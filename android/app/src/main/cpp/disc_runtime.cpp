#include "disc_runtime.h"

#include <cctype>
#include <iomanip>
#include <sstream>

#include "rarc_archive.h"

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

bool read_and_probe_rarc(const GameCubeDisc& disc,
                         const char* path,
                         std::ostringstream* out) {
    std::vector<std::uint8_t> archive_data;
    std::string error;
    if (!disc.read_file(path, &archive_data, &error)) {
        *out << " " << path << " read failed=" << error;
        return false;
    }

    RarcArchive archive;
    if (!archive.open(archive_data, &error)) {
        *out << " " << path << " RARC parse failed=" << error;
        return false;
    }

    *out << " " << path
         << " RARC nodes=" << archive.node_count()
         << " entries=" << archive.file_entry_count()
         << " root=[";

    const auto& entries = archive.root_entries();
    const std::size_t limit = entries.size() < 6 ? entries.size() : 6;
    for (std::size_t i = 0; i < limit; ++i) {
        if (i != 0) *out << ",";
        *out << entries[i].name;
    }
    if (entries.size() > limit) *out << ",...";
    *out << "]";
    return true;
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

    read_and_probe_rarc(disc_, "/forest_1st.arc", &probe);
    read_and_probe_rarc(disc_, "/forest_2nd.arc", &probe);
    read_and_probe_rarc(disc_, "/famicom.arc", &probe);

    std::vector<std::uint8_t> rel;
    if (disc_.read_file("/foresta.rel.szs", &rel, &read_error)) {
        probe << " /foresta.rel.szs=" << rel.size()
              << " magic=" << printable_prefix(rel, 4);
    } else {
        probe << " /foresta.rel.szs read failed=" << read_error;
    }

    last_probe_ = probe.str();
    mounted_ = true;
    if (error) error->clear();
    return true;
}

} // namespace open_crossing

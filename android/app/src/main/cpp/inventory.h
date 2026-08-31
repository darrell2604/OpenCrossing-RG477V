#pragma once

#include <array>
#include <cstdint>

namespace open_crossing {

struct InventorySlot {
    std::uint32_t item_id = 0;
    std::uint16_t count = 0;
};

class Inventory {
public:
    static constexpr std::size_t kSlotCount = 15;
    static constexpr std::uint16_t kMaxStack = 99;

    void reset();
    bool add(std::uint32_t item_id, std::uint16_t count = 1);
    bool remove(std::uint32_t item_id, std::uint16_t count = 1);
    bool contains(std::uint32_t item_id, std::uint16_t count = 1) const;
    std::uint16_t count(std::uint32_t item_id) const;
    bool is_full() const;
    const InventorySlot& slot(std::size_t index) const { return slots_[index]; }

private:
    std::array<InventorySlot, kSlotCount> slots_{};
};

} // namespace open_crossing

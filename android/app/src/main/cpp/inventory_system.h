#pragma once

#include <array>
#include <cstdint>

namespace open_crossing {

struct InventorySlot {
    std::uint32_t item_id = 0;
    std::uint16_t quantity = 0;
    std::uint16_t max_quantity = 99;
};

class InventorySystem {
public:
    static constexpr std::size_t kSlotCount = 15;

    void reset();
    bool add(std::uint32_t item_id, std::uint16_t quantity = 1);
    bool remove(std::uint32_t item_id, std::uint16_t quantity = 1);
    bool contains(std::uint32_t item_id, std::uint16_t quantity = 1) const;
    std::uint16_t count(std::uint32_t item_id) const;
    const InventorySlot& slot(std::size_t index) const { return slots_[index]; }
    std::size_t size() const { return kSlotCount; }

private:
    std::array<InventorySlot, kSlotCount> slots_{};
};

} // namespace open_crossing

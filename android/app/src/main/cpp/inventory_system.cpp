#include "inventory_system.h"

namespace open_crossing {

void InventorySystem::reset() {
    for (auto& slot : slots_) slot = {};
}

std::uint16_t InventorySystem::count(std::uint32_t item_id) const {
    for (const auto& slot : slots_) {
        if (slot.item_id == item_id && slot.quantity > 0) return slot.quantity;
    }
    return 0;
}

bool InventorySystem::contains(std::uint32_t item_id, std::uint16_t quantity) const {
    return count(item_id) >= quantity;
}

bool InventorySystem::add(std::uint32_t item_id, std::uint16_t quantity) {
    if (item_id == 0 || quantity == 0) return false;

    for (auto& slot : slots_) {
        if (slot.item_id == item_id && slot.quantity > 0) {
            const std::uint32_t next = static_cast<std::uint32_t>(slot.quantity) + quantity;
            if (next <= slot.max_quantity) {
                slot.quantity = static_cast<std::uint16_t>(next);
                return true;
            }
            return false;
        }
    }

    for (auto& slot : slots_) {
        if (slot.item_id == 0 || slot.quantity == 0) {
            slot.item_id = item_id;
            slot.quantity = quantity <= slot.max_quantity ? quantity : slot.max_quantity;
            return slot.quantity == quantity;
        }
    }
    return false;
}

bool InventorySystem::remove(std::uint32_t item_id, std::uint16_t quantity) {
    if (!contains(item_id, quantity)) return false;
    for (auto& slot : slots_) {
        if (slot.item_id == item_id && slot.quantity > 0) {
            slot.quantity = static_cast<std::uint16_t>(slot.quantity - quantity);
            if (slot.quantity == 0) slot.item_id = 0;
            return true;
        }
    }
    return false;
}

} // namespace open_crossing

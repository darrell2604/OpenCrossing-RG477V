#pragma once

#include <cstddef>

#include "inventory_system.h"

namespace open_crossing {

class InventorySelection {
public:
    void reset() { selected_ = 0; }

    void move_next(const InventorySystem& inventory) {
        if (inventory.size() == 0) return;
        selected_ = (selected_ + 1) % inventory.size();
    }

    void move_previous(const InventorySystem& inventory) {
        if (inventory.size() == 0) return;
        selected_ = selected_ == 0 ? inventory.size() - 1 : selected_ - 1;
    }

    std::size_t selected() const { return selected_; }
    const InventorySlot& selected_slot(const InventorySystem& inventory) const {
        return inventory.slot(selected_);
    }

private:
    std::size_t selected_ = 0;
};

} // namespace open_crossing

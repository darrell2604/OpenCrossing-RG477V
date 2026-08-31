#include "inventory.h"

namespace open_crossing {

void Inventory::reset() {
    slots_ = {};
}

std::uint16_t Inventory::count(std::uint32_t item_id) const {
    std::uint32_t total = 0;
    for (const auto& s : slots_) {
        if (s.item_id == item_id) total += s.count;
    }
    return total > 0xFFFFu ? 0xFFFFu : static_cast<std::uint16_t>(total);
}

bool Inventory::contains(std::uint32_t item_id, std::uint16_t amount) const {
    return count(item_id) >= amount;
}

bool Inventory::add(std::uint32_t item_id, std::uint16_t amount) {
    if (item_id == 0 || amount == 0) return false;
    std::uint32_t remaining = amount;
    for (auto& s : slots_) {
        if (s.item_id == item_id && s.count < kMaxStack) {
            const auto room = static_cast<std::uint16_t>(kMaxStack - s.count);
            const auto moved = static_cast<std::uint16_t>(remaining < room ? remaining : room);
            s.count = static_cast<std::uint16_t>(s.count + moved);
            remaining -= moved;
            if (remaining == 0) return true;
        }
    }
    for (auto& s : slots_) {
        if (s.item_id == 0) {
            const auto moved = static_cast<std::uint16_t>(remaining < kMaxStack ? remaining : kMaxStack);
            s.item_id = item_id;
            s.count = moved;
            remaining -= moved;
            if (remaining == 0) return true;
        }
    }
    return false;
}

bool Inventory::remove(std::uint32_t item_id, std::uint16_t amount) {
    if (item_id == 0 || amount == 0 || !contains(item_id, amount)) return false;
    std::uint32_t remaining = amount;
    for (auto& s : slots_) {
        if (s.item_id != item_id || s.count == 0) continue;
        const auto moved = static_cast<std::uint16_t>(remaining < s.count ? remaining : s.count);
        s.count = static_cast<std::uint16_t>(s.count - moved);
        remaining -= moved;
        if (s.count == 0) s.item_id = 0;
        if (remaining == 0) return true;
    }
    return false;
}

bool Inventory::is_full() const {
    for (const auto& s : slots_) {
        if (s.item_id == 0 || s.count < kMaxStack) return false;
    }
    return true;
}

} // namespace open_crossing

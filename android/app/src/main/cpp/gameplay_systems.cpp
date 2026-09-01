#include "gameplay_systems.h"

namespace open_crossing {

void GameplaySystems::reset() {
    interaction_count_ = 0;
    last_item_id_ = 0;
    last_bells_ = 0;
}

void GameplaySystems::record_pickup(std::uint32_t item_id, std::uint32_t bells) {
    ++interaction_count_;
    last_item_id_ = item_id;
    last_bells_ = bells;
}

} // namespace open_crossing

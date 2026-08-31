#include "decomp_game_loop.h"

namespace open_crossing {

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    frame_scale_ = 1.0f;
    ready_ = true;
    return ready_;
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;

    const uint64_t elapsed = services.frame_number - tick_;
    tick_ = services.frame_number;
    frame_scale_ = elapsed > 0 ? static_cast<float>(elapsed) : 1.0f;
}

} // namespace open_crossing

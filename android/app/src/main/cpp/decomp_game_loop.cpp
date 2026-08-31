#include "decomp_game_loop.h"

#include "platform_services.h"

namespace open_crossing {

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    ready_ = true;
    return ready_;
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;
    tick_ = services.frame_number;
}

} // namespace open_crossing

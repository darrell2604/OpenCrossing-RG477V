#include "decomp_game_loop.h"

#include "port_controller.h"

namespace open_crossing {

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    frame_scale_ = 1.0f;
    player_.reset();
    ready_ = true;
    return ready_;
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;

    const uint64_t elapsed = services.frame_number - tick_;
    tick_ = services.frame_number;
    frame_scale_ = elapsed > 0 ? static_cast<float>(elapsed) : 1.0f;

    // First portable gameplay slice: consume the same controller abstraction
    // used by the Android backend and advance the upstream-style player walk
    // state without depending on GameCube OS or renderer services.
    player_.update(oc::controller_state());
}

} // namespace open_crossing

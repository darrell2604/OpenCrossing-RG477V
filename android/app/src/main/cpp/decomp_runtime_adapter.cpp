#include "decomp_runtime_adapter.h"

namespace open_crossing {

bool DecompRuntimeAdapter::initialise(const PlatformServices& services) {
    last_frame_ = services.frame_number;
    stage_ = DecompBootStage::platform_ready;

    // The original startup path depends on several GameCube services
    // (threads, IRQs, controller manager and graphics startup). Android owns
    // those responsibilities, so advance through explicit replacement
    // boundaries instead of attempting to compile the hardware layer.
    stage_ = DecompBootStage::frame_system_ready;
    stage_ = DecompBootStage::input_ready;
    stage_ = DecompBootStage::game_loop_ready;
    return true;
}

void DecompRuntimeAdapter::begin_frame(const PlatformServices& services) {
    if (stage_ != DecompBootStage::game_loop_ready) return;
    if (services.frame_number <= last_frame_) return;
    last_frame_ = services.frame_number;
}

} // namespace open_crossing

#pragma once

#include <cstdint>

#include "platform_services.h"

namespace open_crossing {

enum class DecompBootStage : uint8_t {
    cold = 0,
    platform_ready,
    frame_system_ready,
    input_ready,
    game_loop_ready,
};

// Android-side adapter for the upstream game's startup contract.  It models
// the platform services the original entry path expects without importing
// GameCube OS/thread/IRQ implementations into the Android build.
class DecompRuntimeAdapter {
public:
    bool initialise(const PlatformServices& services);
    void begin_frame(const PlatformServices& services);
    DecompBootStage stage() const { return stage_; }

private:
    DecompBootStage stage_ = DecompBootStage::cold;
    uint64_t last_frame_ = 0;
};

} // namespace open_crossing

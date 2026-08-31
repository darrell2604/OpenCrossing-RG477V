#pragma once

#include "decomp_runtime_adapter.h"
#include "platform_services.h"

namespace open_crossing {

class GameRuntime {
public:
    bool initialise();
    void resize(int width, int height);
    void frame();

    bool ready() const { return ready_; }
    const PlatformServices& platform() const { return platform_; }
    const DecompRuntimeAdapter& decomp() const { return decomp_; }

private:
    bool ready_ = false;
    PlatformServices platform_{};
    DecompRuntimeAdapter decomp_{};
    int width_ = 0;
    int height_ = 0;
    unsigned long long frame_counter_ = 0;
};

} // namespace open_crossing

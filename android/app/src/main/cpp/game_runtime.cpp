#include "game_runtime.h"

namespace open_crossing {

bool GameRuntime::initialise() {
    // Platform bootstrap only for now. The decompilation is integrated after
    // its host-pointer assumptions have been audited and isolated.
    ready_ = true;
    return ready_;
}

void GameRuntime::resize(int width, int height) {
    width_ = width;
    height_ = height;
}

void GameRuntime::frame() {
    if (!ready_) {
        return;
    }
    // Game tick/update and GX translation will be connected here.
}

} // namespace open_crossing

#pragma once

#include "gc_memory_types.h"

namespace open_crossing {

class GameRuntime {
public:
    bool initialise();
    void resize(int width, int height);
    void frame();

    bool ready() const { return ready_; }

private:
    bool ready_ = false;
    int width_ = 0;
    int height_ = 0;
};

} // namespace open_crossing

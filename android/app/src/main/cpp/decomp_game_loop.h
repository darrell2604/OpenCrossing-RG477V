#pragma once

#include <cstdint>
#include "platform_services.h"

namespace open_crossing {

class DecompGameLoop {
public:
    bool initialise(const PlatformServices& services);
    void update(const PlatformServices& services);
    uint64_t tick() const { return tick_; }
    bool ready() const { return ready_; }

private:
    bool ready_ = false;
    uint64_t tick_ = 0;
};

} // namespace open_crossing

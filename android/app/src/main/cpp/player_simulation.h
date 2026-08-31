#pragma once

#include <cstdint>

#include "port_controller.h"

namespace open_crossing {

// Portable player movement slice derived from the upstream player-walk path.
// It deliberately keeps collision, animation and rendering out of this layer
// so the movement contract can execute on Android without GameCube services.
class PlayerSimulation {
public:
    void reset();
    void update(const oc::ControllerState& controller);

    float x() const { return x_; }
    float z() const { return z_; }
    float angle() const { return angle_; }
    float speed() const { return speed_; }
    std::uint64_t steps() const { return steps_; }

private:
    float x_ = 0.0f;
    float z_ = 0.0f;
    float angle_ = 0.0f;
    float speed_ = 0.0f;
    std::uint64_t steps_ = 0;
};

} // namespace open_crossing

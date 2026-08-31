#pragma once

#include <cmath>

#include "port_controller.h"
#include "player_simulation.h"

namespace open_crossing {

struct CameraState {
    float x = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
    float target_x = 0.0f;
    float target_z = 0.0f;
    float distance = 6.0f;
    float height = 4.0f;
    float pitch = 0.0f;
};

class PlayerCamera {
public:
    void reset(const PlayerSimulation& player);
    void update(const PlayerSimulation& player, const oc::ControllerState& controller);
    const CameraState& state() const { return state_; }

private:
    static float approach(float current, float target, float amount);
    CameraState state_{};
};

} // namespace open_crossing

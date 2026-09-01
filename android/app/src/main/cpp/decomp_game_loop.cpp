#include "decomp_game_loop.h"

#include "port_controller.h"

namespace open_crossing {

namespace {
constexpr std::uint32_t kPickupItemId = 1001u;
constexpr std::uint32_t kBellReward = 1u;
}

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    frame_scale_ = 1.0f;
    state_.reset();
    state_.start_new_day();
    inventory_.reset();
    player_.reset();
    ready_ = true;
    return true;
}

void DecompGameLoop::advance_game_state(std::uint64_t elapsed_frames) {
    if (state_.state().phase != GamePhase::Playing || elapsed_frames == 0) return;
    state_.update(elapsed_frames);
}

void DecompGameLoop::process_actions() {
    const auto& controller = oc::controller_state();

    if ((controller.buttons & oc::START) != 0u) {
        state_.toggle_pause();
        return;
    }

    if (state_.state().phase != GamePhase::Playing) return;

    if ((controller.buttons & oc::A) != 0u) {
        if (inventory_.add(kPickupItemId, 1)) {
            state_.record_interaction();
            state_.add_bells(kBellReward);
        }
    }
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;

    const std::uint64_t elapsed = services.frame_number - tick_;
    tick_ = services.frame_number;
    frame_scale_ = static_cast<float>(elapsed);

    process_actions();
    if (state_.state().phase != GamePhase::Playing) return;

    player_.update(oc::controller_state());
    advance_game_state(elapsed);
}

} // namespace open_crossing

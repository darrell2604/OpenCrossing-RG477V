#include "decomp_game_loop.h"

#include "port_controller.h"

namespace open_crossing {

namespace {
constexpr std::uint32_t kFramesPerGameDay = 60u * 60u * 24u;
constexpr std::uint32_t kMaxBells = 999999u;
}

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    frame_scale_ = 1.0f;
    phase_ = GamePhase::Playing;
    day_ = 1;
    bells_ = 0;
    interaction_count_ = 0;
    frames_since_day_start_ = 0;
    player_.reset();
    ready_ = true;
    return ready_;
}

void DecompGameLoop::advance_game_time() {
    if (phase_ != GamePhase::Playing) return;

    ++frames_since_day_start_;
    if (frames_since_day_start_ >= kFramesPerGameDay) {
        frames_since_day_start_ = 0;
        ++day_;
    }
}

void DecompGameLoop::process_gameplay_state() {
    if (phase_ != GamePhase::Playing) return;

    // Keep the portable game-state model deterministic and side-effect-light.
    // Interaction rewards are intentionally bounded until the real inventory/
    // economy systems are wired in from the recovered game code.
    if (player_.moving() && player_.steps() > 0 && (player_.steps() % 600u) == 0u) {
        if (bells_ < kMaxBells) ++bells_;
    }
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;

    const uint64_t elapsed = services.frame_number - tick_;
    tick_ = services.frame_number;
    frame_scale_ = elapsed > 0 ? static_cast<float>(elapsed) : 1.0f;

    if (oc::controller_state().buttons & oc::START) {
        phase_ = phase_ == GamePhase::Paused ? GamePhase::Playing : GamePhase::Paused;
    }

    if (phase_ == GamePhase::Playing) {
        player_.update(oc::controller_state());
        advance_game_time();
        process_gameplay_state();
    }
}

} // namespace open_crossing

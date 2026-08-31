#include "decomp_game_loop.h"

#include "port_controller.h"

namespace open_crossing {

namespace {
constexpr std::uint64_t kFramesPerGameDay = 60ull * 60ull * 24ull;
constexpr std::uint64_t kFramesPerGameMinute = 60ull;
constexpr std::uint64_t kMaxBells = 999999ull;
constexpr std::uint32_t kPickupItemId = 1001u;
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

void DecompGameLoop::advance_game_time(std::uint64_t elapsed_frames) {
    if (phase_ != GamePhase::Playing) return;
    frames_since_day_start_ += static_cast<std::uint32_t>(elapsed_frames);
    while (frames_since_day_start_ >= kFramesPerGameDay) {
        frames_since_day_start_ -= static_cast<std::uint32_t>(kFramesPerGameDay);
        ++day_;
    }
}

void DecompGameLoop::process_gameplay_state() {
    if (phase_ != GamePhase::Playing) return;
    if (player_.moving() && player_.steps() > 0 && (player_.steps() % 600u) == 0u) {
        if (bells_ < kMaxBells) ++bells_;
    }
}

void DecompGameLoop::process_actions() {
    if (phase_ != GamePhase::Playing) return;

    const auto& controller = oc::controller_state();
    if (controller.buttons & oc::A) {
        ++interaction_count_;
        inventory_.add(kPickupItemId, 1);
    }
}

void DecompGameLoop::update(const PlatformServices& services) {
    if (!ready_ || services.frame_number <= tick_) return;

    const std::uint64_t elapsed = services.frame_number - tick_;
    tick_ = services.frame_number;
    frame_scale_ = elapsed > 0 ? static_cast<float>(elapsed) : 1.0f;

    if (oc::controller_state().buttons & oc::START) {
        phase_ = phase_ == GamePhase::Paused ? GamePhase::Playing : GamePhase::Paused;
    }

    if (phase_ != GamePhase::Playing) return;

    player_.update(oc::controller_state());
    process_actions();
    advance_game_time(elapsed);
    process_gameplay_state();
}

} // namespace open_crossing

#include "game_state.h"

#include <algorithm>
#include <limits>

namespace open_crossing {

void GameStateSystem::reset() {
    state_ = {};
    state_.phase = GamePhase::Boot;
    state_.day = 1;
}

void GameStateSystem::start_new_day() {
    state_.day = std::max<std::uint32_t>(1, state_.day + 1);
    state_.game_minutes = 0;
    state_.phase = GamePhase::Playing;
}

void GameStateSystem::update(std::uint64_t elapsed_frames) {
    if (state_.phase != GamePhase::Playing || elapsed_frames == 0) return;
    constexpr std::uint64_t kFramesPerGameMinute = 60;
    const std::uint64_t minutes = elapsed_frames / kFramesPerGameMinute;
    if (minutes == 0) return;
    state_.game_minutes += minutes;
}

void GameStateSystem::toggle_pause() {
    if (state_.phase == GamePhase::Playing) {
        state_.phase = GamePhase::Paused;
    } else if (state_.phase == GamePhase::Paused) {
        state_.phase = GamePhase::Playing;
    }
}

void GameStateSystem::add_bells(std::uint32_t amount) {
    const std::uint64_t max_bells = std::numeric_limits<std::uint64_t>::max();
    state_.bells = (max_bells - state_.bells < amount) ? max_bells : state_.bells + amount;
}

bool GameStateSystem::spend_bells(std::uint32_t amount) {
    if (state_.bells < amount) return false;
    state_.bells -= amount;
    return true;
}

void GameStateSystem::record_interaction() {
    ++state_.interactions;
}

void GameStateSystem::set_inventory_open(bool open) {
    state_.inventory_open = open;
}

} // namespace open_crossing

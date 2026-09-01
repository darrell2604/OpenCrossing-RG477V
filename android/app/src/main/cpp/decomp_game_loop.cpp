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
    previous_buttons_ = 0;
    interaction_target_id_ = 0;
    selected_inventory_slot_ = 0;
    ready_ = true;
    return true;
}

void DecompGameLoop::advance_game_state(std::uint64_t elapsed_frames) {
    if (state_.state().phase != GamePhase::Playing || elapsed_frames == 0) return;
    state_.update(elapsed_frames);
}

void DecompGameLoop::move_inventory_selection(int delta) {
    constexpr std::size_t count = InventorySystem::kSlotCount;
    if (count == 0) return;
    const int current = static_cast<int>(selected_inventory_slot_);
    int next = current + delta;
    while (next < 0) next += static_cast<int>(count);
    while (next >= static_cast<int>(count)) next -= static_cast<int>(count);
    selected_inventory_slot_ = static_cast<std::size_t>(next);
}

void DecompGameLoop::process_actions() {
    const auto& controller = oc::controller_state();
    const bool just_pressed_a = (controller.buttons & oc::A) != 0u &&
                                (previous_buttons_ & oc::A) == 0u;
    const bool just_pressed_start = (controller.buttons & oc::START) != 0u &&
                                    (previous_buttons_ & oc::START) == 0u;
    const bool just_pressed_y = (controller.buttons & oc::Y) != 0u &&
                                (previous_buttons_ & oc::Y) == 0u;
    previous_buttons_ = controller.buttons;

    if (just_pressed_start) {
        state_.toggle_pause();
        state_.set_inventory_open(false);
        return;
    }

    if (just_pressed_y && state_.state().phase == GamePhase::Playing) {
        state_.set_inventory_open(!state_.state().inventory_open);
        if (state_.state().inventory_open) selected_inventory_slot_ = 0;
    }

    if (state_.state().phase != GamePhase::Playing) return;

    if (state_.state().inventory_open) {
        if ((controller.dpad & oc::DPAD_LEFT) != 0u &&
            (previous_buttons_ & 0x10000u) == 0u) {
            move_inventory_selection(-1);
        }
        if ((controller.dpad & oc::DPAD_RIGHT) != 0u) {
            move_inventory_selection(1);
        }
        if ((controller.dpad & oc::DPAD_UP) != 0u) {
            move_inventory_selection(-1);
        }
        if ((controller.dpad & oc::DPAD_DOWN) != 0u) {
            move_inventory_selection(1);
        }
        if (just_pressed_a) {
            const auto& slot = inventory_.slot(selected_inventory_slot_);
            if (slot.quantity > 0) {
                inventory_.remove(slot.item_id, 1);
            }
        }
        return;
    }

    if (just_pressed_a) {
        const std::uint32_t target = interaction_target_id_ == 0 ? kPickupItemId : interaction_target_id_;
        if (inventory_.add(target, 1)) {
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
    if (state_.state().phase != GamePhase::Playing || state_.state().inventory_open) return;

    player_.update(oc::controller_state());
    advance_game_state(elapsed);
}

} // namespace open_crossing

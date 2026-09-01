#include "decomp_game_loop.h"

#include "port_controller.h"

namespace open_crossing {

namespace {
constexpr std::uint32_t kPickupItemId = 1001u;
constexpr std::uint32_t kBellReward = 1u;
constexpr std::size_t kInventorySlots = 15u;
}

bool DecompGameLoop::initialise(const PlatformServices& services) {
    tick_ = services.frame_number;
    frame_scale_ = 1.0f;
    state_.reset();
    state_.start_new_day();
    inventory_.reset();
    player_.reset();
    selected_slot_ = 0;
    previous_dpad_ = 0;
    ready_ = true;
    return true;
}

void DecompGameLoop::advance_game_state(std::uint64_t elapsed_frames) {
    if (state_.state().phase != GamePhase::Playing || elapsed_frames == 0) return;
    state_.update(elapsed_frames);
}

void DecompGameLoop::process_actions() {
    const auto& controller = oc::controller_state();
    const bool just_pressed_a = (controller.buttons & oc::A) != 0u &&
                                (previous_buttons_ & oc::A) == 0u;
    const bool just_pressed_start = (controller.buttons & oc::START) != 0u &&
                                    (previous_buttons_ & oc::START) == 0u;
    const std::uint8_t dpad = controller.dpad;
    const bool just_up = (dpad & oc::DPAD_UP) != 0u && (previous_dpad_ & oc::DPAD_UP) == 0u;
    const bool just_down = (dpad & oc::DPAD_DOWN) != 0u && (previous_dpad_ & oc::DPAD_DOWN) == 0u;
    const bool just_left = (dpad & oc::DPAD_LEFT) != 0u && (previous_dpad_ & oc::DPAD_LEFT) == 0u;
    const bool just_right = (dpad & oc::DPAD_RIGHT) != 0u && (previous_dpad_ & oc::DPAD_RIGHT) == 0u;

    previous_buttons_ = controller.buttons;
    previous_dpad_ = dpad;

    if (just_pressed_start) {
        state_.toggle_pause();
        state_.set_inventory_open(false);
        return;
    }

    if (state_.state().phase != GamePhase::Playing) return;

    if (state_.state().inventory_open) {
        if (just_right || just_down) {
            selected_slot_ = (selected_slot_ + 1u) % kInventorySlots;
        } else if (just_left || just_up) {
            selected_slot_ = (selected_slot_ + kInventorySlots - 1u) % kInventorySlots;
        }

        if (just_pressed_a) {
            const InventorySlot& selected = inventory_.slot(selected_slot_);
            if (selected.item_id != 0u && selected.quantity > 0u &&
                inventory_.remove(selected.item_id, 1)) {
                state_.record_interaction();
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

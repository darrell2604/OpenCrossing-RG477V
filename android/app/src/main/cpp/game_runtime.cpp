#include "game_runtime.h"

#include <algorithm>

namespace open_crossing {

namespace {
void seed_demo_world(WorldGeometry& world) {
    world.reset();
    world.set_ground({-8.0f, -8.0f, 8.0f, 8.0f, 0.0f});
    world.add_object({2.0f, 0.0f, 1.0f, 1.5f, 0.35f, 1});
    world.add_object({-2.0f, 1.5f, 1.2f, 0.9f, -0.55f, 2});
}
}

bool GameRuntime::initialise() {
    ready_ = false;
    platform_ = {};
    scene_ = {};
    frame_counter_ = 0;

    interaction_targets_.clear();
    interaction_targets_.push_back({1.5f, 0.0f, 0.8f, 1});
    interaction_.reset();
    interaction_.set_targets(interaction_targets_);

    world_.reset();
    seed_demo_world(world_);

    if (!decomp_.initialise(platform_)) return false;
    if (!game_loop_.initialise(platform_)) return false;
    camera_.reset(game_loop_.player());
    if (!renderer_.initialise()) return false;

    if (width_ > 0 && height_ > 0) {
        renderer_.resize(width_, height_);
    }

    ready_ = true;
    return true;
}

void GameRuntime::resize(int width, int height) {
    width_ = std::max(1, width);
    height_ = std::max(1, height);
    platform_.viewport_width = width_;
    platform_.viewport_height = height_;
    renderer_.resize(width_, height_);
}

void GameRuntime::frame() {
    if (!ready_) return;

    platform_.frame_number = ++frame_counter_;
    decomp_.begin_frame(platform_);
    game_loop_.update(platform_);

    const auto& player = game_loop_.player();
    const auto& controls = oc::controller_state();
    interaction_.update(player, controls);
    camera_.update(player, controls);

    scene_.player_x = player.x();
    scene_.player_z = player.z();
    scene_.player_angle = player.angle();
    scene_.frame = platform_.frame_number;

    RenderFrame render_frame{};
    render_frame.player_x = player.x();
    render_frame.player_z = player.z();
    render_frame.player_angle = player.angle();
    render_frame.camera_x = camera_.state().x;
    render_frame.camera_z = camera_.state().z;
    render_frame.camera_yaw = camera_.state().yaw;
    render_frame.world = &world_;
    render_frame.world_vertices = &world_.vertices();
    renderer_.draw(render_frame);
}

void GameRuntime::shutdown() {
    renderer_.shutdown();
    ready_ = false;
}

} // namespace open_crossing

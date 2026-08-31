#include "world_geometry.h"

#include <cmath>

namespace open_crossing {

namespace {
WorldVertex make_vertex(float x, float y, float z, float u, float v) {
    return WorldVertex{x, y, z, u, v};
}
}

void WorldGeometry::reset() {
    ground_ = {};
    objects_.clear();
    vertices_.clear();
}

void WorldGeometry::set_ground(const WorldQuad& ground) {
    ground_ = ground;
    rebuild_mesh();
}

void WorldGeometry::add_object(const WorldObject& object) {
    objects_.push_back(object);
    rebuild_mesh();
}

void WorldGeometry::rebuild_mesh() {
    vertices_.clear();
    vertices_.reserve(6 + objects_.size() * 6);

    vertices_.push_back(make_vertex(ground_.min_x, ground_.y, ground_.min_z, 0.0f, 0.0f));
    vertices_.push_back(make_vertex(ground_.max_x, ground_.y, ground_.min_z, 1.0f, 0.0f));
    vertices_.push_back(make_vertex(ground_.max_x, ground_.y, ground_.max_z, 1.0f, 1.0f));
    vertices_.push_back(make_vertex(ground_.min_x, ground_.y, ground_.min_z, 0.0f, 0.0f));
    vertices_.push_back(make_vertex(ground_.max_x, ground_.y, ground_.max_z, 1.0f, 1.0f));
    vertices_.push_back(make_vertex(ground_.min_x, ground_.y, ground_.max_z, 0.0f, 1.0f));

    for (const WorldObject& object : objects_) {
        const float half = object.size * 0.5f;
        const float s = std::sin(object.yaw);
        const float c = std::cos(object.yaw);
        const float local_x[4] = {-half, half, half, -half};
        const float local_z[4] = {-half, -half, half, half};
        float x[4]{};
        float z[4]{};
        for (int i = 0; i < 4; ++i) {
            x[i] = object.x + local_x[i] * c - local_z[i] * s;
            z[i] = object.z + local_x[i] * s + local_z[i] * c;
        }

        const float y0 = ground_.y;
        const float y1 = ground_.y + object.height;
        vertices_.push_back(make_vertex(x[0], y0, z[0], 0.0f, 0.0f));
        vertices_.push_back(make_vertex(x[1], y0, z[1], 1.0f, 0.0f));
        vertices_.push_back(make_vertex(x[2], y1, z[2], 1.0f, 1.0f));
        vertices_.push_back(make_vertex(x[0], y0, z[0], 0.0f, 0.0f));
        vertices_.push_back(make_vertex(x[2], y1, z[2], 1.0f, 1.0f));
        vertices_.push_back(make_vertex(x[3], y1, z[3], 0.0f, 1.0f));
    }
}

} // namespace open_crossing

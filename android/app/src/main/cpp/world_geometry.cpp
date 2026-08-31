#include "world_geometry.h"

namespace open_crossing {

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

    // Ground quad. Two triangles are emitted as an explicit vertex stream so
    // the renderer can later upload this data without needing platform APIs.
    vertices_.push_back({ground_.min_x, ground_.y, ground_.min_z, 0.0f, 0.0f});
    vertices_.push_back({ground_.max_x, ground_.y, ground_.min_z, 1.0f, 0.0f});
    vertices_.push_back({ground_.max_x, ground_.y, ground_.max_z, 1.0f, 1.0f});
    vertices_.push_back({ground_.min_x, ground_.y, ground_.min_z, 0.0f, 0.0f});
    vertices_.push_back({ground_.max_x, ground_.y, ground_.max_z, 1.0f, 1.0f});
    vertices_.push_back({ground_.min_x, ground_.y, ground_.max_z, 0.0f, 1.0f});

    // Simple billboard-ready object footprint. The renderer can turn this
    // position/size data into model geometry later without changing the world
    // representation.
    for (const WorldObject& object : objects_) {
        const float half = object.size * 0.5f;
        const float y0 = ground_.y;
        const float y1 = ground_.y + object.height;
        vertices_.push_back({object.x - half, y0, object.z - half, 0.0f, 0.0f});
        vertices_.push_back({object.x + half, y0, object.z - half, 1.0f, 0.0f});
        vertices_.push_back({object.x + half, y1, object.z + half, 1.0f, 1.0f});
        vertices_.push_back({object.x - half, y0, object.z - half, 0.0f, 0.0f});
        vertices_.push_back({object.x + half, y1, object.z + half, 1.0f, 1.0f});
        vertices_.push_back({object.x - half, y1, object.z + half, 0.0f, 1.0f});
    }
}

} // namespace open_crossing

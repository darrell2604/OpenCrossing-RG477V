#include "world_geometry.h"

namespace open_crossing {

void WorldGeometry::reset() {
    ground_ = {};
    objects_.clear();
}

void WorldGeometry::set_ground(const WorldQuad& ground) {
    ground_ = ground;
}

void WorldGeometry::add_object(const WorldObject& object) {
    objects_.push_back(object);
}

} // namespace open_crossing

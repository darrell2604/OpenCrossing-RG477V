#pragma once

#include <cstdint>
#include <vector>

namespace open_crossing {

struct WorldVertex {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
};

struct WorldQuad {
    float min_x = 0.0f;
    float min_z = 0.0f;
    float max_x = 0.0f;
    float max_z = 0.0f;
    float y = 0.0f;
};

struct WorldObject {
    float x = 0.0f;
    float z = 0.0f;
    float size = 0.5f;
    float height = 0.0f;
    float yaw = 0.0f;
    std::uint32_t id = 0;
};

class WorldGeometry {
public:
    void reset();
    void set_ground(const WorldQuad& ground);
    void add_object(const WorldObject& object);
    void rebuild_mesh();

    const WorldQuad& ground() const { return ground_; }
    const std::vector<WorldObject>& objects() const { return objects_; }
    const std::vector<WorldVertex>& vertices() const { return vertices_; }

private:
    WorldQuad ground_{};
    std::vector<WorldObject> objects_;
    std::vector<WorldVertex> vertices_;
};

} // namespace open_crossing

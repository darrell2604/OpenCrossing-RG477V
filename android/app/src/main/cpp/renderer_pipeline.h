#pragma once

#include <cstdint>
#include <vector>

#include <GLES3/gl3.h>

#include "world_geometry.h"

namespace open_crossing {

struct RenderFrame {
    float player_x = 0.0f;
    float player_z = 0.0f;
    float player_angle = 0.0f;
    float camera_x = 0.0f;
    float camera_z = 0.0f;
    float camera_yaw = 0.0f;
    const WorldGeometry* world = nullptr;
};

class RendererPipeline {
public:
    bool initialise();
    void resize(int width, int height);
    void draw(const RenderFrame& frame);
    void shutdown();

private:
    void upload_world(const WorldGeometry& world);

    GLuint program_ = 0;
    GLuint player_vertex_buffer_ = 0;
    GLuint world_vertex_buffer_ = 0;
    GLsizei world_vertex_count_ = 0;
    GLint translation_location_ = -1;
    GLint angle_location_ = -1;
    GLint colour_location_ = -1;
    GLint view_location_ = -1;
    int width_ = 1;
    int height_ = 1;
    const WorldGeometry* uploaded_world_ = nullptr;
    std::size_t uploaded_vertex_count_ = 0;
};

} // namespace open_crossing

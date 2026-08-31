#pragma once

#include <GLES3/gl3.h>

namespace open_crossing {

struct RenderFrame {
    float player_x = 0.0f;
    float player_z = 0.0f;
    float player_angle = 0.0f;
    float camera_x = 0.0f;
    float camera_z = 0.0f;
    float camera_yaw = 0.0f;
};

class RendererPipeline {
public:
    bool initialise();
    void resize(int width, int height);
    void draw(const RenderFrame& frame);
    void shutdown();

private:
    GLuint program_ = 0;
    GLuint vertex_buffer_ = 0;
    GLint translation_location_ = -1;
    GLint angle_location_ = -1;
    GLint colour_location_ = -1;
    int width_ = 1;
    int height_ = 1;
};

} // namespace open_crossing

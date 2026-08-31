#include "renderer_pipeline.h"

#include <algorithm>
#include <cmath>

namespace open_crossing {
namespace {
GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint make_program() {
    constexpr char kVertex[] = R"(
        #version 300 es
        layout(location = 0) in vec2 a_position;
        uniform vec2 u_translation;
        uniform float u_angle;
        void main() {
            float c = cos(u_angle);
            float s = sin(u_angle);
            vec2 p = vec2(a_position.x * c - a_position.y * s,
                          a_position.x * s + a_position.y * c);
            gl_Position = vec4(p + u_translation, 0.0, 1.0);
        }
    )";
    constexpr char kFragment[] = R"(
        #version 300 es
        precision mediump float;
        uniform vec4 u_colour;
        out vec4 frag_colour;
        void main() { frag_colour = u_colour; }
    )";
    const GLuint vertex = compile_shader(GL_VERTEX_SHADER, kVertex);
    const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, kFragment);
    if (!vertex || !fragment) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return 0;
    }
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
} // namespace

bool RendererPipeline::initialise() {
    if (program_) return true;
    program_ = make_program();
    if (!program_) return false;

    constexpr float triangle[] = {
        0.0f, 0.065f,
        -0.045f, -0.04f,
        0.045f, -0.04f,
    };
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    translation_location_ = glGetUniformLocation(program_, "u_translation");
    angle_location_ = glGetUniformLocation(program_, "u_angle");
    colour_location_ = glGetUniformLocation(program_, "u_colour");
    return true;
}

void RendererPipeline::resize(int width, int height) {
    width_ = std::max(1, width);
    height_ = std::max(1, height);
    glViewport(0, 0, width_, height_);
}

void RendererPipeline::draw(const RenderFrame& frame) {
    if (!program_ || !vertex_buffer_) return;

    glClearColor(0.035f, 0.075f, 0.045f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Camera-relative projection for the portable scene path. The real
    // GameCube camera/renderer can replace this transform without changing
    // the game-state contract.
    const float dx = frame.player_x - frame.camera_x;
    const float dz = frame.player_z - frame.camera_z;
    const float c = std::cos(-frame.camera_yaw);
    const float s = std::sin(-frame.camera_yaw);
    const float view_x = dx * c - dz * s;
    const float view_z = dx * s + dz * c;
    const float scale_x = 0.06f;
    const float scale_y = 0.06f * static_cast<float>(width_) / static_cast<float>(height_);

    glUseProgram(program_);
    glUniform2f(translation_location_,
                std::clamp(view_x * scale_x, -0.9f, 0.9f),
                std::clamp(view_z * scale_y, -0.9f, 0.9f));
    glUniform1f(angle_location_, frame.player_angle - frame.camera_yaw);
    glUniform4f(colour_location_, 0.25f, 0.65f, 0.35f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void RendererPipeline::shutdown() {
    if (vertex_buffer_) {
        glDeleteBuffers(1, &vertex_buffer_);
        vertex_buffer_ = 0;
    }
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

} // namespace open_crossing

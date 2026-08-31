#include "game_runtime.h"

#include <algorithm>
#include <cmath>

#include <GLES3/gl3.h>

namespace open_crossing {

namespace {
GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) { glDeleteShader(shader); return 0; }
    return shader;
}

GLuint make_program() {
    constexpr char kVertexShader[] = R"(
        #version 300 es
        layout(location = 0) in vec2 a_position;
        uniform vec2 u_translation;
        uniform float u_angle;
        void main() {
            float c = cos(u_angle);
            float s = sin(u_angle);
            vec2 rotated = vec2(
                a_position.x * c - a_position.y * s,
                a_position.x * s + a_position.y * c
            );
            gl_Position = vec4(rotated + u_translation, 0.0, 1.0);
        }
    )";
    constexpr char kFragmentShader[] = R"(
        #version 300 es
        precision mediump float;
        out vec4 frag_colour;
        void main() { frag_colour = vec4(0.25, 0.65, 0.35, 1.0); }
    )";
    const GLuint vertex = compile_shader(GL_VERTEX_SHADER, kVertexShader);
    const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, kFragmentShader);
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
    if (linked != GL_TRUE) { glDeleteProgram(program); return 0; }
    return program;
}
} // namespace

bool GameRuntime::initialise() {
    ready_ = false;
    platform_ = {};
    scene_ = {};
    frame_counter_ = 0;
    interaction_targets_.clear();
    interaction_targets_.push_back({1.5f, 0.0f, 0.8f, 1});
    interaction_.reset();
    interaction_.set_targets(interaction_targets_);
    if (!decomp_.initialise(platform_)) return false;
    if (!game_loop_.initialise(platform_)) return false;
    camera_.reset(game_loop_.player());
    ready_ = true;
    return true;
}

void GameRuntime::resize(int width, int height) {
    width_ = width;
    height_ = height > 0 ? height : 1;
    platform_.viewport_width = width_;
    platform_.viewport_height = height_;
    glViewport(0, 0, width_, height_);
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

    static GLuint program = 0;
    static GLuint vertex_buffer = 0;
    static GLint translation_location = -1;
    static GLint angle_location = -1;

    if (!program) {
        program = make_program();
        constexpr float vertices[] = {
            0.0f, 0.065f,
            -0.045f, -0.04f,
            0.045f, -0.04f
        };
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (program) {
            translation_location = glGetUniformLocation(program, "u_translation");
            angle_location = glGetUniformLocation(program, "u_angle");
        }
    }

    glClearColor(0.04f, 0.10f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (program && vertex_buffer) {
        const float screen_x = std::clamp(scene_.player_x * 0.035f, -0.88f, 0.88f);
        const float screen_y = std::clamp(scene_.player_z * 0.035f, -0.88f, 0.88f);

        glUseProgram(program);
        if (translation_location >= 0) {
            glUniform2f(translation_location, screen_x, screen_y);
        }
        if (angle_location >= 0) {
            glUniform1f(angle_location, scene_.player_angle);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}

} // namespace open_crossing

#include "game_runtime.h"

#include <GLES3/gl3.h>

namespace open_crossing {

namespace {

GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
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
    constexpr char kVertexShader[] = R"(
        #version 300 es
        layout(location = 0) in vec2 a_position;
        void main() { gl_Position = vec4(a_position, 0.0, 1.0); }
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
    if (linked != GL_TRUE) {
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

} // namespace

bool GameRuntime::initialise() {
    ready_ = true;
    platform_ = {};
    return ready_;
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

    static GLuint program = 0;
    static GLuint vertex_buffer = 0;
    if (!program) {
        program = make_program();
        constexpr float vertices[] = { 0.0f, 0.65f, -0.65f, -0.55f, 0.65f, -0.55f };
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glClearColor(0.04f, 0.10f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (program && vertex_buffer) {
        glUseProgram(program);
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

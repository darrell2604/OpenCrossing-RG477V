#include "renderer_pipeline.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

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
        layout(location = 0) in vec3 a_position;
        layout(location = 1) in vec2 a_uv;
        uniform vec3 u_translation;
        uniform float u_angle;
        uniform vec2 u_view_scale;
        out vec2 v_uv;
        void main() {
            float c = cos(u_angle);
            float s = sin(u_angle);
            vec3 rotated = vec3(
                a_position.x * c - a_position.z * s,
                a_position.y,
                a_position.x * s + a_position.z * c
            );
            vec3 view = rotated + u_translation;
            gl_Position = vec4(view.x * u_view_scale.x, view.z * u_view_scale.y, 0.0, 1.0);
            v_uv = a_uv;
        }
    )";
    constexpr char kFragment[] = R"(
        #version 300 es
        precision mediump float;
        in vec2 v_uv;
        uniform vec4 u_colour;
        out vec4 frag_colour;
        void main() {
            float shade = 0.88 + 0.12 * (1.0 - clamp(abs(v_uv.x - 0.5) + abs(v_uv.y - 0.5), 0.0, 1.0));
            frag_colour = vec4(u_colour.rgb * shade, u_colour.a);
        }
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

struct GpuVertex {
    float x;
    float y;
    float z;
    float u;
    float v;
};
} // namespace

bool RendererPipeline::initialise() {
    if (program_) return true;

    program_ = make_program();
    if (!program_) return false;

    constexpr GpuVertex player[] = {
        {0.0f, 0.0f, 0.09f, 0.5f, 1.0f},
        {-0.06f, 0.0f, -0.06f, 0.0f, 0.0f},
        {0.06f, 0.0f, -0.06f, 1.0f, 0.0f},
    };

    glGenBuffers(1, &player_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, player_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(player), player, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &world_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    translation_location_ = glGetUniformLocation(program_, "u_translation");
    angle_location_ = glGetUniformLocation(program_, "u_angle");
    colour_location_ = glGetUniformLocation(program_, "u_colour");
    view_location_ = glGetUniformLocation(program_, "u_view_scale");
    return true;
}

void RendererPipeline::resize(int width, int height) {
    width_ = std::max(1, width);
    height_ = std::max(1, height);
    glViewport(0, 0, width_, height_);
}

void RendererPipeline::upload_world(const WorldGeometry& world) {
    const auto& vertices = world.vertices();
    if (uploaded_world_ == &world && uploaded_vertex_count_ == vertices.size()) return;

    std::vector<GpuVertex> gpu_vertices;
    gpu_vertices.reserve(vertices.size());
    for (const WorldVertex& vertex : vertices) {
        gpu_vertices.push_back({vertex.x, vertex.y, vertex.z, vertex.u, vertex.v});
    }

    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(gpu_vertices.size() * sizeof(GpuVertex)),
                 gpu_vertices.empty() ? nullptr : gpu_vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    world_vertex_count_ = static_cast<GLsizei>(gpu_vertices.size());
    uploaded_world_ = &world;
    uploaded_vertex_count_ = vertices.size();
}

void RendererPipeline::draw(const RenderFrame& frame) {
    if (!program_ || !player_vertex_buffer_ || !world_vertex_buffer_) return;

    glClearColor(0.035f, 0.075f, 0.045f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    const float view_scale_x = 0.06f;
    const float view_scale_y = 0.06f * static_cast<float>(width_) / static_cast<float>(height_);
    const float c = std::cos(-frame.camera_yaw);
    const float s = std::sin(-frame.camera_yaw);

    glUseProgram(program_);
    glUniform2f(view_location_, view_scale_x, view_scale_y);

    if (frame.world) {
        upload_world(*frame.world);
        glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer_);
        glUniform3f(translation_location_, -frame.camera_x, 0.0f, -frame.camera_z);
        glUniform1f(angle_location_, 0.0f);
        glUniform4f(colour_location_, 0.20f, 0.42f, 0.24f, 1.0f);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GpuVertex), nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GpuVertex), reinterpret_cast<const void*>(offsetof(GpuVertex, u)));
        glDrawArrays(GL_TRIANGLES, 0, world_vertex_count_);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, player_vertex_buffer_);
    const float dx = frame.player_x - frame.camera_x;
    const float dz = frame.player_z - frame.camera_z;
    const float view_x = dx * c - dz * s;
    const float view_z = dx * s + dz * c;
    glUniform3f(translation_location_, view_x, 0.0f, view_z);
    glUniform1f(angle_location_, frame.player_angle - frame.camera_yaw);
    glUniform4f(colour_location_, 0.25f, 0.65f, 0.35f, 1.0f);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GpuVertex), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GpuVertex), reinterpret_cast<const void*>(offsetof(GpuVertex, u)));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void RendererPipeline::shutdown() {
    if (world_vertex_buffer_) {
        glDeleteBuffers(1, &world_vertex_buffer_);
        world_vertex_buffer_ = 0;
    }
    if (player_vertex_buffer_) {
        glDeleteBuffers(1, &player_vertex_buffer_);
        player_vertex_buffer_ = 0;
    }
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    uploaded_world_ = nullptr;
    uploaded_vertex_count_ = 0;
    world_vertex_count_ = 0;
}

} // namespace open_crossing

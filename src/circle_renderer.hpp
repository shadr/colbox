#pragma once
#include "components.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "pch.hpp"
#include "raylib.h"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <GLES3/gl3.h>
#include <rlgl.h>

#define check_errors()                                                         \
  {                                                                            \
    auto error = glGetError();                                                 \
    while (error != GL_NO_ERROR) {                                             \
      std::cout << "[" << __LINE__ << "]: " << error << std::endl;             \
      error = glGetError();                                                    \
    }                                                                          \
  }

struct CircleData {
  float x;
  float y;
  float radius;
  glm::vec4 color;
};

std::string read_file(const std::filesystem::path &path);

struct CircleRenderer {
  static unsigned int vao;
  static unsigned int vbo;
  static unsigned int ebo;
  static unsigned int instance_vbo;
  static unsigned int shader;
  static size_t length;
  static int viewproj_loc;

  static void init() {
    auto vertex_shader = read_file("../src/shader_vertex.glsl");
    auto fragment_shader = read_file("../src/shader_fragment.glsl");

    unsigned int vshader =
        rlCompileShader(vertex_shader.c_str(), RL_VERTEX_SHADER);
    unsigned int fshader =
        rlCompileShader(fragment_shader.c_str(), RL_FRAGMENT_SHADER);
    shader = rlLoadShaderProgram(vshader, fshader);
    check_errors();
    viewproj_loc = rlGetLocationUniform(shader, "viewproj");
    check_errors();

    // clang-format off
    float vertices[] = {
        1.0f,  1.0f,
        1.0f,  -1.0f,
        -1.0f, -1.0f,
        -1.0f, 1.0f,
    };

    unsigned short indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    // clang-format on

    vao = rlLoadVertexArray();
    rlEnableVertexArray(vao);

    vbo = rlLoadVertexBuffer(&vertices, sizeof(vertices), false);
    ebo = rlLoadVertexBufferElement(&indices, sizeof(indices), false);

    instance_vbo = rlLoadVertexBuffer(nullptr, 1024 * 1024, false);

    rlEnableVertexAttribute(0);
    rlEnableVertexBuffer(vbo);
    rlSetVertexAttribute(0, 2, RL_FLOAT, false, 2 * sizeof(float), 0);
    check_errors();
    rlDisableVertexBuffer();

    // instance position
    rlEnableVertexAttribute(1);
    rlEnableVertexBuffer(instance_vbo);
    rlSetVertexAttribute(1, 2, RL_FLOAT, false, sizeof(CircleData), 0);
    check_errors();
    rlDisableVertexBuffer();
    rlSetVertexAttributeDivisor(1, 1);

    // radius
    rlEnableVertexAttribute(2);
    rlEnableVertexBuffer(instance_vbo);
    rlSetVertexAttribute(2, 1, RL_FLOAT, false, sizeof(CircleData),
                         (void *)(offsetof(CircleData, radius)));
    check_errors();
    rlDisableVertexBuffer();
    rlSetVertexAttributeDivisor(2, 1);

    // color
    rlEnableVertexAttribute(3);
    rlEnableVertexBuffer(instance_vbo);
    rlSetVertexAttribute(3, 4, RL_FLOAT, false, sizeof(CircleData),
                         (void *)(offsetof(CircleData, color)));
    check_errors();
    rlDisableVertexBuffer();
    rlSetVertexAttributeDivisor(3, 1);

    check_errors();

    rlDisableVertexArray();
  }

  static void update(const CircleData *ptr, size_t length,
                     const glm::mat4 &viewproj) {
    rlEnableShader(shader);

    glUniformMatrix4fv(viewproj_loc, 1, false, (float *)&viewproj);
    check_errors();
    rlEnableVertexArray(vao);
    rlUpdateVertexBuffer(instance_vbo, reinterpret_cast<const void *>(ptr),
                         length * sizeof(CircleData), 0);
    check_errors();
    rlDisableVertexArray();
    CircleRenderer::length = length;
  }

  static void draw() {
    if (length == 0)
      return;

    rlEnableVertexArray(vao);
    rlEnableShader(shader);
    rlDrawVertexArrayElementsInstanced(0, 6, 0, length);
    check_errors();
    rlDisableShader();
    rlDisableVertexArray();
  }
};

void draw_circles(entt::registry &reg, const glm::mat4 &viewproj);

#pragma once
#include "pch.hpp"
#include "tool.hpp"
#include "ui.hpp"

void spawn_new_body(entt::registry &registry, b2WorldId physicsId);

struct Game {
  b2WorldId physicsId;
  entt::registry ecs;

  Ui ui_drawer{*this};

  bool enable_render = true;
  bool enable_physics = true;

  glm::mat4 proj;
  glm::mat4 view;
  glm::mat4 viewproj;

  float gravity[2] = {0.0f, 9.81f};

  BaseTool *current_tool = new ForceTool();

  void init_world();
  void loop();
  void update_gravity();

  Game();
  ~Game() { b2DestroyWorld(physicsId); }

  void recalulate_view_projection();
  void recreate_walls();
};

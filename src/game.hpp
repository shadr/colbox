#pragma once
#include "pch.hpp"

void spawn_new_body(entt::registry &registry, b2WorldId physicsId);

struct Game {
  b2WorldId physicsId;
  entt::registry ecs;

  bool enable_render = true;
  bool enable_physics = true;

  glm::mat4 proj;
  glm::mat4 view;
  glm::mat4 viewproj;

  void init_world();
  void loop();

  Game();
  ~Game() { b2DestroyWorld(physicsId); }

  void recalulate_view_projection();
  void recreate_walls();
};

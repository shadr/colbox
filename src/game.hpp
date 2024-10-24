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

  float gravity[2] = {0.0f, 9.81f};

  void init_world();
  void loop();
  void update_gravity();

  Game();
  ~Game() { b2DestroyWorld(physicsId); }

  void recalulate_view_projection();
  void recreate_walls();
};

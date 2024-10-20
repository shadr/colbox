#pragma once
#include "pch.hpp"

void spawn_new_body(entt::registry &registry, b2WorldId physicsId);

struct Game {
  b2WorldId physicsId;
  entt::registry ecs;

  bool enable_render = true;
  bool enable_physics = true;

  void init_world();
  void loop();

  ~Game() { b2DestroyWorld(physicsId); }
};

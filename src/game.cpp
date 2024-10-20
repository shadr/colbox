#include "game.hpp"
#include "components.hpp"
#include "random_ranges.hpp"
#include "stats.hpp"
#include "systems.hpp"
#include "ui.hpp"

void spawn_wall(b2WorldId physicsId, float x, float y, float width,
                float height) {
  auto wallDef = b2DefaultBodyDef();
  wallDef.type = b2_staticBody;
  wallDef.position = b2Vec2{x, y};
  auto wallId = b2CreateBody(physicsId, &wallDef);
  auto wallBox = b2MakeBox(width, height);
  auto wallShapeDef = b2DefaultShapeDef();
  b2CreatePolygonShape(wallId, &wallShapeDef, &wallBox);
}

void spawn_new_body(entt::registry &registry, b2WorldId physicsId) {
  const auto entity = registry.create();
  registry.emplace<ColorComponent>(entity);

  auto bodyDef = b2DefaultBodyDef();
  bodyDef.type = b2_dynamicBody;
  bodyDef.gravityScale = 1.0;
  // bodyDef.linearDamping = 0.1f;
  bodyDef.position =
      b2Vec2{static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng),
             static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng)};
  bodyDef.linearVelocity =
      b2Vec2{std::cos(dis_vel_angle(rng)), std::sin(dis_vel_angle(rng))} *
      150.0f;
  auto bodyId = b2CreateBody(physicsId, &bodyDef);
  b2Circle dynamicCircle;
  dynamicCircle.center = b2Vec2{0.0f, 0.0f};
  dynamicCircle.radius = dis_size(rng);
  auto shapeDef = b2DefaultShapeDef();
  shapeDef.density = 1.0f;
  shapeDef.friction = 0.0f;
  // shapeDef.restitution = 1.0f;
  b2CreateCircleShape(bodyId, &shapeDef, &dynamicCircle);
  registry.emplace<PhysicsComponent>(entity, bodyId, dynamicCircle.radius);
}

void Game::init_world() {
  spawn_wall(physicsId, 0.0f, 0.0f, 4.0f, GetScreenHeight());
  spawn_wall(physicsId, GetScreenWidth(), 0.0f, 4.0f, GetScreenHeight());
  spawn_wall(physicsId, 0.0f, 0.0f, GetScreenWidth(), 4.0f);
  spawn_wall(physicsId, 0.0f, GetScreenHeight(), GetScreenWidth(), 4.0f);

  for (int i = 0; i < 50; i++) {
    spawn_new_body(ecs, physicsId);
  }
}

void Game::loop() {
  BeginDrawing();
  ClearBackground(Color{16, 16, 16, 255});

  auto dt = GetFrameTime();

  auto now = std::chrono::steady_clock().now().time_since_epoch();
  if (enable_physics)
  b2World_Step(physicsId, dt, 4);
  auto physics_step_time =
      std::chrono::steady_clock().now().time_since_epoch() - now;

  remove_escaped_circles(ecs);

  if (!ImGui::GetIO().WantCaptureMouse)
    mouse_interaction_system(physicsId);

  now = std::chrono::steady_clock().now().time_since_epoch();
  color_system(ecs, dt);
  auto color_system_time =
      std::chrono::steady_clock().now().time_since_epoch() - now;
  now = std::chrono::steady_clock().now().time_since_epoch();
  if (enable_render)
    draw_system(ecs);
  auto draw_system_time =
      std::chrono::steady_clock().now().time_since_epoch() - now;

  auto counters = b2World_GetCounters(physicsId);

  Statistics stats{draw_system_time, color_system_time, physics_step_time,
                   counters};

  draw_ui(*this, stats, dt);

  DrawFPS(10, 10);
  EndDrawing();
}

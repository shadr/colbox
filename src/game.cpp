#include "game.hpp"
#include "box2d/box2d.h"
#include "circle_renderer.hpp"
#include "components.hpp"
#include "random_ranges.hpp"
#include "stats.hpp"
#include "systems.hpp"
#include "ui.hpp"

Game::Game() : ecs(entt::registry()) {
  auto physicsDef = b2DefaultWorldDef();
  physicsDef.gravity = b2Vec2{gravity[0], gravity[1]};
  physicsId = b2CreateWorld(&physicsDef);
  recalulate_view_projection();
}

void Game::recalulate_view_projection() {
  float w = (float)GetScreenWidth();
  float h = (float)GetScreenHeight();
  proj = glm::ortho(-w / 2.f, w / 2.f, -h / 2.f, h / 2.f, -10.0f, 10.0f);
  view = glm::lookAt(glm::vec3(w, h, -4.f) / 2.0f, glm::vec3(w, h, 0.f) / 2.0f,
                     glm::vec3(0.0, -1.0, 0.0));
  viewproj = proj * view;
}

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
  bodyDef.isBullet = true;
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

void Game::recreate_walls() {
  // TODO: do not recreate walls, instead move existing walls to the new
  // position remove walls
  auto view = ecs.view<WallMarker>();
  view.each([this](entt::entity e, WallMarker &wall) {
    ecs.destroy(e);
    b2DestroyBody(wall.id);
  });

  // spawn walls
  float w = (float)GetScreenWidth();
  float h = (float)GetScreenHeight();
  float arr[][4] = {
      {0.0f, 0.0f, 4.0f, h},
      {w, 0.0f, 4.0f, h},
      {0.0f, 0.0f, w, 4.0f},
      {0.0f, h, w, 4.0f},
  };
  for (auto [x, y, width, height] : arr) {
    auto wallDef = b2DefaultBodyDef();
    wallDef.type = b2_staticBody;
    wallDef.position = b2Vec2{x, y};
    auto wallId = b2CreateBody(physicsId, &wallDef);
    auto wallBox = b2MakeBox(width, height);
    auto wallShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(wallId, &wallShapeDef, &wallBox);

    auto entity = ecs.create();
    ecs.emplace<WallMarker>(entity, wallId);
  }
}

void Game::init_world() {
  recreate_walls();

  for (int i = 0; i < 50; i++) {
    spawn_new_body(ecs, physicsId);
  }
}

void Game::update_gravity() {
  b2World_SetGravity(physicsId, reinterpret_cast<b2Vec2 &>(gravity));
}

void Game::loop() {
  if (IsWindowResized()) {
    recalulate_view_projection();
    recreate_walls();
  }

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
    mouse_interaction_system(physicsId, tool, tool_radius, paint_data);

  now = std::chrono::steady_clock().now().time_since_epoch();
  color_system(ecs, dt);
  auto color_system_time =
      std::chrono::steady_clock().now().time_since_epoch() - now;
  now = std::chrono::steady_clock().now().time_since_epoch();
  if (enable_render)
    draw_circles(ecs, viewproj);
  auto draw_system_time =
      std::chrono::steady_clock().now().time_since_epoch() - now;

  auto counters = b2World_GetCounters(physicsId);

  Statistics stats{draw_system_time, color_system_time, physics_step_time,
                   counters};

  if (tool != Tool::None) {
    rlSetLineWidth(0.25f);
    DrawCircleLinesV(GetMousePosition(), tool_radius, RAYWHITE);
  }

  ui_drawer.draw_ui(stats, dt);

  DrawFPS(10, 10);
  EndDrawing();
}

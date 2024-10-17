#include <chrono>
#include <numbers>
#include <random>

#include "box2d/types.h"
#include "pch.hpp"

#include <box2d/box2d.h>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(2.0, 4.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

rtree tree;
b2WorldId physicsId;

struct PhysicsComponent {
  b2BodyId id;
  float radius;

  PhysicsComponent(b2BodyId id, float r) : id(id), radius(r) {}
};

struct ColorComponent {
  float hue;
  ColorComponent() : hue(dis_start_color(rng)) {}
};

void color_system(entt::registry &reg, float dt) {
  const auto view = reg.view<ColorComponent>();
  view.each([dt](ColorComponent &c) {
    c.hue += 36.0 * dt;
    if (c.hue > 360.0) {
      c.hue = 0.0;
    }
  });
}

void draw_system(entt::registry &reg) {
  const auto view = reg.view<const PhysicsComponent, const ColorComponent>();
  view.each([](const PhysicsComponent &p, const ColorComponent &c) {
    Color color = ColorFromHSV(c.hue, 1.0, 1.0);
    auto pos = b2Body_GetPosition(p.id);
    DrawCircle(pos.x, pos.y, p.radius, color);
  });
}

struct OverlapContext {
  b2Vec2 origin;
  float multiplier;
};

bool overlapCallback(b2ShapeId shapeId, void *ctxptr) {
  OverlapContext context = *(OverlapContext *)ctxptr;
  b2BodyId bodyId = b2Shape_GetBody(shapeId);
  auto position = b2Body_GetPosition(bodyId);
  auto distance_squared = b2DistanceSquared(position, context.origin);
  auto dir = b2Normalize(position - context.origin);
  auto force = dir * 2e10f;
  force.x /= distance_squared;
  force.y /= distance_squared;
  b2Body_ApplyForceToCenter(bodyId, force * context.multiplier, true);
  return true;
}

void mouse_interaction_system() {
  const float force_radius = 300.0;
  float force_direction = 0.0f;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    force_direction += 1.0f;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    force_direction -= 1.0f;
  }
  if (force_direction != 0.0f) {
    auto mouse_pos = GetMousePosition();
    b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
    b2Circle circle = {origin, force_radius};
    OverlapContext context{origin, force_direction};
    b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                          b2DefaultQueryFilter(), &overlapCallback, &context);
  }
}

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  rlImGuiSetup(true);

  auto physicsDef = b2DefaultWorldDef();
  physicsDef.gravity = b2Vec2{0.0f, 0.0};
  physicsId = b2CreateWorld(&physicsDef);

  entt::registry registry;

  auto create_wall = [](float x, float y, float width, float height) {
    auto wallDef = b2DefaultBodyDef();
    wallDef.position = b2Vec2{x, y};
    auto wallId = b2CreateBody(physicsId, &wallDef);
    auto wallBox = b2MakeBox(width, height);
    auto wallShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(wallId, &wallShapeDef, &wallBox);
  };

  create_wall(0.0f, 0.0f, 4.0f, GetScreenHeight());
  create_wall(GetScreenWidth(), 0.0f, 4.0f, GetScreenHeight());
  create_wall(0.0f, 0.0f, GetScreenWidth(), 4.0f);
  create_wall(0.0f, GetScreenHeight(), GetScreenWidth(), 4.0f);

  auto spawn_new_body = [&registry]() {
    const auto entity = registry.create();
    registry.emplace<ColorComponent>(entity);

    auto bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;

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
    shapeDef.restitution = 1.0f;
    b2CreateCircleShape(bodyId, &shapeDef, &dynamicCircle);
    registry.emplace<PhysicsComponent>(entity, bodyId, dynamicCircle.radius);
  };

  for (int i = 0; i < 50; i++) {
    spawn_new_body();
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{16, 16, 16, 255});

    auto dt = GetFrameTime();
    b2World_Step(physicsId, dt, 4);

    mouse_interaction_system();
    color_system(registry, dt);
    draw_system(registry);

    rlImGuiBegin();
    auto b = ImGui::Button("click me");

    ImGui::LabelText("", "Entities: %zu",
                     registry.view<entt::entity>().size_hint());
    if (b) {
      for (int i = 0; i < 500; i++) {
        spawn_new_body();
      }
    }
    rlImGuiEnd();

    DrawFPS(10, 10);
    EndDrawing();
  }

  b2DestroyWorld(physicsId);
  rlImGuiShutdown();
  CloseWindow();
}

#include <chrono>
#include <numbers>
#include <random>

#include "pch.hpp"

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(2.0, 2.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

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

void color_system(entt::registry &reg, float) {
  const auto view = reg.view<PhysicsComponent, ColorComponent>();
  view.each([](PhysicsComponent &p, ColorComponent &c) {
    auto vel = b2Body_GetLinearVelocity(p.id);
    auto radians = std::atan2(vel.y, vel.x);
    auto degrees = radians * 180.0 / std::numbers::pi;
    c.hue = degrees;
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
  OverlapContext context = *reinterpret_cast<OverlapContext *>(ctxptr);
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

void remove_escaped_circles(entt::registry &reg) {
  const auto view = reg.view<PhysicsComponent>();
  const auto screen_width = GetScreenWidth();
  const auto screen_height = GetScreenHeight();
  view.each(
      [&reg, screen_width, screen_height](entt::entity e, PhysicsComponent &p) {
        auto pos = b2Body_GetPosition(p.id);
        if (pos.x < 0.0 || pos.x > screen_width || pos.y < 0.0 ||
            pos.y > screen_height) {
          b2DestroyBody(p.id);
          reg.destroy(e);
        }
      });
}

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  rlImGuiSetup(true);

  auto physicsDef = b2DefaultWorldDef();
  physicsDef.gravity = b2Vec2{0.0f, 9.81};
  physicsId = b2CreateWorld(&physicsDef);

  entt::registry registry;

  auto create_wall = [](float x, float y, float width, float height) {
    auto wallDef = b2DefaultBodyDef();
    wallDef.type = b2_staticBody;
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
    bodyDef.gravityScale = 1.0;
    bodyDef.linearDamping = 0.1f;
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
    shapeDef.friction = 1.0f;
    shapeDef.restitution = 0.0f;
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

    auto now = std::chrono::steady_clock().now().time_since_epoch();
    b2World_Step(physicsId, dt, 4);
    auto physics_step_time =
        std::chrono::steady_clock().now().time_since_epoch() - now;

    remove_escaped_circles(registry);

    if (!ImGui::GetIO().WantCaptureMouse)
      mouse_interaction_system();

    now = std::chrono::steady_clock().now().time_since_epoch();
    color_system(registry, dt);
    auto color_system_time =
        std::chrono::steady_clock().now().time_since_epoch() - now;
    now = std::chrono::steady_clock().now().time_since_epoch();
    draw_system(registry);
    auto draw_system_time =
        std::chrono::steady_clock().now().time_since_epoch() - now;

    auto counters = b2World_GetCounters(physicsId);

    rlImGuiBegin();
    auto b = ImGui::Button("click me");

    ImGui::Text("Entities: %zu", registry.view<entt::entity>().size_hint());

    ImGui::Text("Physics bodies: %i", counters.bodyCount);
    ImGui::Text("Delta time: %f ms", dt * 1000.0f);
    ImGui::Text("Physics time: %f ms",
                static_cast<float>(physics_step_time.count()) / 1000000.0f);
    ImGui::Text("Color time: %f ms",
                static_cast<float>(color_system_time.count()) / 1000000.0f);
    ImGui::Text("Draw time: %f ms",
                static_cast<float>(draw_system_time.count()) / 1000000.0f);

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

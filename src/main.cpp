#include "components.hpp"
#include "pch.hpp"
#include "random_ranges.hpp"
#include "systems.hpp"

b2WorldId physicsId;

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
      mouse_interaction_system(physicsId);

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

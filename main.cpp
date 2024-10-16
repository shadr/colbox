#include "rlImGui/rlImGui.h"
#include <chrono>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <imgui.h>
#include <iostream>
#include <numbers>
#include <random>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(8.0, 16.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

struct PositionComponent {
  float x, y, radius;

  PositionComponent()
      : x(static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng)),
        y(static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng)),
        radius(dis_size(rng)) {}
};

struct VelocityComponent {
  float x;
  float y;

  VelocityComponent()
      : x(std::cos(dis_vel_angle(rng)) * 150.0),
        y(std::sin(dis_vel_angle(rng)) * 150.0) {}
};

struct ColorComponent {
  float hue;
  ColorComponent() : hue(dis_start_color(rng)) {}
};

void move_system(entt::registry &reg, float dt) {
  const auto view = reg.view<PositionComponent, const VelocityComponent>();

  view.each([dt](PositionComponent &p, const VelocityComponent &v) {
    p.x += v.x * dt;
    p.y += v.y * dt;
  });
}

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
  const auto view = reg.view<const PositionComponent, const ColorComponent>();
  view.each([](const PositionComponent &p, const ColorComponent &c) {
    Color color = ColorFromHSV(c.hue, 1.0, 1.0);
    DrawCircle(p.x, p.y, p.radius, color);
  });
}

void wall_collision_system(entt::registry &reg) {
  const auto view = reg.view<PositionComponent, VelocityComponent>();
  const auto screen_width = GetScreenWidth();
  const auto screen_height = GetScreenHeight();
  view.each([screen_width, screen_height](PositionComponent &p,
                                          VelocityComponent &v) {
    if (p.x - p.radius < 0) {
      p.x = p.radius;
      v.x *= -1;
    } else if (p.x + p.radius > GetScreenWidth()) {
      p.x = screen_width - p.radius;
      v.x *= -1;
    }
    if (p.y - p.radius < 0) {
      p.y = p.radius;
      v.y *= -1;
    } else if (p.y + p.radius > GetScreenHeight()) {
      p.y = screen_height - p.radius;
      v.y *= -1;
    }
  });
}

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  rlImGuiSetup(true);

  entt::registry registry;
  for (int i = 0; i < 500; i++) {
    const auto entity = registry.create();
    registry.emplace<PositionComponent>(entity);
    registry.emplace<VelocityComponent>(entity);
    registry.emplace<ColorComponent>(entity);
  }

  // world.system<PositionComponent, VelocityComponent>("SquareCollision")
  //     .kind(flecs::OnValidate)
  //     .multi_threaded()
  //     .write<VelocityComponent>()
  //     .read<PositionComponent>()
  //     .run([](flecs::iter &it) {
  //       while (it.next()) {
  //         auto p = it.field<PositionComponent>(0);
  //         auto v = it.field<VelocityComponent>(1);
  //         for (auto i : it) {
  //           auto &p1 = p[i];
  //           auto &v1 = v[i];
  //           auto &r1 = reinterpret_cast<Rectangle &>(p1);
  //           for (auto j : it) {
  //             if (i == j) {
  //               continue;
  //             }
  //             auto &p2 = p[j];
  //             auto &v2 = v[j];
  //             auto &r2 = reinterpret_cast<Rectangle &>(p2);
  //             bool collide = CheckCollisionRecs(r1, r2);
  //
  //             if (collide) {
  //               auto r = GetCollisionRec(r1, r2);
  //               if (r.width > r.height) {
  //                 v1.y *= -1;
  //                 v2.y *= -1;
  //               } else {
  //                 v1.x *= -1;
  //                 v2.x *= -1;
  //               }
  //             }
  //           }
  //         }
  //       }
  //     });

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{16, 16, 16, 255});

    auto dt = GetFrameTime();

    move_system(registry, dt);
    color_system(registry, dt);
    draw_system(registry);
    wall_collision_system(registry);

    rlImGuiBegin();
    auto b = ImGui::Button("click me");

    ImGui::LabelText("", "Entities: %zu",
                     registry.view<entt::entity>().size_hint());
    if (b) {
      for (int i = 0; i < 500; i++) {
        const auto entity = registry.create();
        registry.emplace<PositionComponent>(entity);
        registry.emplace<VelocityComponent>(entity);
        registry.emplace<ColorComponent>(entity);
      }
    }
    rlImGuiEnd();

    DrawFPS(10, 10);
    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow();
}

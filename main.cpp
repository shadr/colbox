#include "rlImGui/rlImGui.h"
#include <chrono>
#include <flecs.h>
#include <imgui.h>
#include <iostream>
#include <numbers>
#include <random>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(50.0, 150.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

struct Square {
  float x;
  float y;
  float width;
  float height;
  float vx = 3.5;
  float vy = 3.5;
  float hue = 0.0;

  Square()
      : x(static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng)),
        y(static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng)),
        width(dis_size(rng)), height(width) {}

  void update() {
    hue += 0.5;
    if (hue > 360.0) {
      hue = 0.0;
    }

    x += vx;
    y += vy;
  }

  void draw() {
    Color color = ColorFromHSV(hue, 1.0, 1.0);
    DrawRectangle(x, y, width, height, color);
  }

  void collision_with_screen() {
    if (x < 0 || x + width > GetScreenWidth()) {
      vx *= -1;
    }

    if (y < 0 || y + height > GetScreenHeight()) {
      vy *= -1;
    }
  }

  void collision_with_squares(Square &other) {
    auto r1 = Rectangle{x, y, width, height};
    auto r2 = Rectangle{other.x, other.y, other.width, other.height};
    bool collide = CheckCollisionRecs(r1, r2);

    if (collide) {
      auto r = GetCollisionRec(r1, r2);
      if (r.width > r.height) {
        vy *= -1;
        other.vy *= -1;
      } else {
        vx *= -1;
        other.vx *= -1;
      }
    }
  }
};

struct PositionComponent {
  float x, y, width, height;

  PositionComponent()
      : x(static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng)),
        y(static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng)),
        width(dis_size(rng)), height(width) {}
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

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  rlImGuiSetup(true);

  flecs::world world;
  world.set_threads(8);
  for (int i = 0; i < 10; i++) {
    auto e = world.entity();
    e.set(PositionComponent()).set(VelocityComponent()).set(ColorComponent());
  }

  world.system<PositionComponent, const VelocityComponent>("Move")
      .kind(flecs::OnUpdate)
      .multi_threaded()
      .write<PositionComponent>()
      .read<VelocityComponent>()
      .each([](flecs::iter &it, size_t, PositionComponent &p,
               const VelocityComponent &v) {
        p.x += v.x * it.delta_time();
        p.y += v.y * it.delta_time();
      });

  world.system<PositionComponent, VelocityComponent>("WallCollision")
      .multi_threaded()
      .kind(flecs::OnValidate)
      .write<PositionComponent, VelocityComponent>()
      .read<PositionComponent>()
      .each([](PositionComponent &p, VelocityComponent &v) {
        if (p.x < 0) {
          p.x = 0;
          v.x *= -1;
        } else if (p.x + p.width > GetScreenWidth()) {
          p.x = GetScreenWidth() - p.width;
          v.x *= -1;
        }
        if (p.y < 0) {
          p.y = 0;
          v.y *= -1;
        } else if (p.y + p.height > GetScreenHeight()) {
          p.y = GetScreenHeight() - p.height;
          v.y *= -1;
        }
      });

  world.system<PositionComponent, VelocityComponent>("SquareCollision")
      .kind(flecs::OnValidate)
      .write<VelocityComponent>()
      .read<PositionComponent>()
      .run([](flecs::iter &it) {
        while (it.next()) {
          auto p = it.field<PositionComponent>(0);
          auto v = it.field<VelocityComponent>(1);
          auto count = it.count();
          for (unsigned long i = 0; i < count; i++) {
            auto &p1 = p[i];
            auto &v1 = v[i];
            auto &r1 = reinterpret_cast<Rectangle &>(p1);
            for (unsigned long j = i + 1; j < count; j++) {
              auto &p2 = p[j];
              auto &v2 = v[j];
              auto &r2 = reinterpret_cast<Rectangle &>(p2);
              bool collide = CheckCollisionRecs(r1, r2);

              if (collide) {
                auto r = GetCollisionRec(r1, r2);
                if (r.width > r.height) {
                  v1.y *= -1;
                  v2.y *= -1;
                } else {
                  v1.x *= -1;
                  v2.x *= -1;
                }
              }
            }
          }
        }
      });

  world.system<ColorComponent>("ColorChange")
      .multi_threaded()
      .kind(flecs::OnUpdate)
      .write<ColorComponent>()
      .each([](flecs::iter &it, size_t, ColorComponent &c) {
        c.hue += 36.0 * it.delta_time();
        if (c.hue > 360.0) {
          c.hue = 0.0;
        }
      });

  world.system<PositionComponent, ColorComponent>("DrawSystem")
      .read<PositionComponent, ColorComponent>()
      .each([](PositionComponent &p, ColorComponent &c) {
        Color color = ColorFromHSV(c.hue, 1.0, 1.0);
        DrawRectangle(p.x, p.y, p.width, p.height, color);
      });

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{16, 16, 16, 255});

    auto dt = GetFrameTime();

    world.progress(dt);

    rlImGuiBegin();
    auto b = ImGui::Button("click me");

    ImGui::LabelText("", "Entities: %i", world.count<ColorComponent>());
    if (b) {
      for (int i = 0; i < 500; i++) {
        world.entity()
            .set(PositionComponent())
            .set(VelocityComponent())
            .set(ColorComponent());
      }
    }
    rlImGuiEnd();

    DrawFPS(10, 10);
    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow();
}

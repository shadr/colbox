#include "rlImGui/rlImGui.h"
#include <chrono>
#include <flecs.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <numbers>
#include <random>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(20.0, 40.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

struct PositionComponent {
  glm::vec2 pos;
  float radius;

  PositionComponent()
      : pos(glm::vec2{
            static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng),
            static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng)}),
        radius(dis_size(rng)) {}
};

struct VelocityComponent {
  glm::vec2 vel;

  VelocityComponent()
      : vel(glm::vec2{std::cos(dis_vel_angle(rng)),
                      std::sin(dis_vel_angle(rng))} *
            150.0f) {}
};

struct ColorComponent {
  float hue;
  ColorComponent() : hue(dis_start_color(rng)) {}
};

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raylibapp");
  SetTargetFPS(60);
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
      .each(
          [](flecs::iter &it, size_t, PositionComponent &p,
             const VelocityComponent &v) { p.pos += v.vel * it.delta_time(); });

  world.system<PositionComponent, VelocityComponent>("WallCollision")
      .multi_threaded()
      .kind(flecs::OnValidate)
      .write<PositionComponent, VelocityComponent>()
      .read<PositionComponent>()
      .each([](PositionComponent &p, VelocityComponent &v) {
        if (p.pos.x - p.radius < 0) {
          p.pos.x = p.radius;
          v.vel.x *= -1;
        } else if (p.pos.x + p.radius > GetScreenWidth()) {
          p.pos.x = GetScreenWidth() - p.radius;
          v.vel.x *= -1;
        }
        if (p.pos.y - p.radius < 0) {
          p.pos.y = p.radius;
          v.vel.y *= -1;
        } else if (p.pos.y + p.radius > GetScreenHeight()) {
          p.pos.y = GetScreenHeight() - p.radius;
          v.vel.y *= -1;
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
            for (unsigned long j = i + 1; j < count; j++) {
              auto &p2 = p[j];
              auto &v2 = v[j];
              float distance = glm::distance(p1.pos, p2.pos);
              bool collide = distance <= p1.radius + p2.radius;

              if (collide) {
                float overlap_length = distance - p1.radius - p2.radius;
                glm::vec2 n = glm::normalize(p2.pos - p1.pos);
                p1.pos += n * overlap_length / 2.0f;
                p2.pos -= n * overlap_length / 2.0f;
                float p = glm::dot(v1.vel, n) - glm::dot(v2.vel, n);
                v1.vel = v1.vel - p * n;
                v2.vel = v2.vel + p * n;
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
        DrawCircle(p.pos.x, p.pos.y, p.radius, color);
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
      for (int i = 0; i < 200; i++) {
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

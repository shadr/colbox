#include "rlImGui/rlImGui.h"
#include <chrono>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <numbers>
#include <random>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(4.0, 8.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

struct PositionComponent {
  glm::vec2 pos;
  float radius;

  PositionComponent()
      : pos(glm::vec2(
            static_cast<float>(GetScreenWidth()) / 2 + dis_position(rng),
            static_cast<float>(GetScreenHeight()) / 2 + dis_position(rng))),
        radius(dis_size(rng)) {}
};

struct VelocityComponent {
  glm::vec2 vel;

  VelocityComponent()
      : vel(glm::vec2(std::cos(dis_vel_angle(rng)),
                      std::sin(dis_vel_angle(rng)) * 150.)) {}
};

struct ColorComponent {
  float hue;
  ColorComponent() : hue(dis_start_color(rng)) {}
};

void move_system(entt::registry &reg, float dt) {
  const auto view = reg.view<PositionComponent, const VelocityComponent>();

  view.each([dt](PositionComponent &p, const VelocityComponent &v) {
    p.pos += v.vel * dt;
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
    DrawCircle(p.pos.x, p.pos.y, p.radius, color);
  });
}

void wall_collision_system(entt::registry &reg) {
  const auto view = reg.view<PositionComponent, VelocityComponent>();
  const auto screen_width = GetScreenWidth();
  const auto screen_height = GetScreenHeight();
  view.each([screen_width, screen_height](PositionComponent &p,
                                          VelocityComponent &v) {
    if (p.pos.x - p.radius < 0) {
      p.pos.x = p.radius;
      v.vel.x *= -1;
    } else if (p.pos.x + p.radius > GetScreenWidth()) {
      p.pos.x = screen_width - p.radius;
      v.vel.x *= -1;
    }
    if (p.pos.y - p.radius < 0) {
      p.pos.y = p.radius;
      v.vel.y *= -1;
    } else if (p.pos.y + p.radius > GetScreenHeight()) {
      p.pos.y = screen_height - p.radius;
      v.vel.y *= -1;
    }
  });
}

void circle_collision_system(entt::registry &reg) {
  const auto view = reg.view<PositionComponent, VelocityComponent>();
  for (auto e1 : view) {
    auto &p1 = view.get<PositionComponent>(e1);
    auto &v1 = view.get<VelocityComponent>(e1);
    for (auto e2 : view) {
      if (e1 == e2) {
        continue;
      }
      auto &p2 = view.get<PositionComponent>(e2);
      auto &v2 = view.get<VelocityComponent>(e2);
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

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{16, 16, 16, 255});

    auto dt = GetFrameTime();

    move_system(registry, dt);
    color_system(registry, dt);
    draw_system(registry);
    wall_collision_system(registry);
    circle_collision_system(registry);

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

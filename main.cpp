#include "rlImGui/rlImGui.h"
#include <chrono>
#include <format>
#include <imgui.h>
#include <iostream>
#include <random>
#include <raylib.h>

auto rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());

std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(50.0, 150.0);

struct UpdateComponent {
  virtual void update() = 0;
};

struct MovementSystem : UpdateComponent {
  void update() override {}
};

struct ColorChangeSystem : UpdateComponent {
  void update() override { std::format("{}", 1); }
};

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

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "raylibapp");
  SetTargetFPS(60);
  rlImGuiSetup(true);

  std::vector<Square> squares = {Square(), Square(), Square()};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{16, 16, 16});

    for (auto &sq : squares) {
      sq.update();
    }

    for (int i = 0; i < squares.size(); i++) {
      auto &sq = squares[i];
      sq.collision_with_screen();
      for (int j = i + 1; j < squares.size(); j++) {
        auto &sq2 = squares[j];
        sq.collision_with_squares(sq2);
      }
    }

    for (auto &sq : squares) {
      sq.draw();
    }

    rlImGuiBegin();
    ImGui::Button("click me");
    for (auto sq : squares) {
      ImGui::LabelText("", "%f %f %f %f", sq.x, sq.y, sq.vx, sq.vy);
    }
    bool open = true;
    rlImGuiEnd();

    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow();
}

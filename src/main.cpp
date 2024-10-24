#include "circle_renderer.hpp"
#include "components.hpp"
#include "game.hpp"
#include "pch.hpp"
#include "random_ranges.hpp"
#include "raylib.h"

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "colbox");
  SetTargetFPS(60);
  rlImGuiSetup(true);

  Game game;
  CircleRenderer::init();

  game.init_world();

  while (!WindowShouldClose()) {
    game.loop();
  }

  rlImGuiShutdown();
  CloseWindow();
}

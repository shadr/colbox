#include "circle_renderer.hpp"
#include "components.hpp"
#include "game.hpp"
#include "pch.hpp"
#include "random_ranges.hpp"

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  rlImGuiSetup(true);

  auto physicsDef = b2DefaultWorldDef();
  physicsDef.gravity = b2Vec2{0.0f, 9.81};

  Game game = {b2CreateWorld(&physicsDef), entt::registry()};
  CircleRenderer::init();

  game.init_world();

  while (!WindowShouldClose()) {
    game.loop();
  }

  rlImGuiShutdown();
  CloseWindow();
}

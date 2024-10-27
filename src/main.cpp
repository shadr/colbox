#include "circle_renderer.hpp"
#include "game.hpp"
#include "pch.hpp"
#include "raylib.h"

#if __EMSCRIPTEN__
#include "emscripten.h"

void game_loop(void *arg) {
  Game *game = static_cast<Game *>(arg);
  game->loop();
}
#endif

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

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(game_loop, &game, 0, 1);
#else
  while (!WindowShouldClose()) {
    game.loop();
  }
#endif

  rlImGuiShutdown();
  CloseWindow();
}

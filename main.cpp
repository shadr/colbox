#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <imgui.h>
#include <string>

int main(int argc, char *args[]) {
  const int screenWidth = 640;
  const int screenHeight = 480;

  std::string a = "something";

  InitWindow(screenWidth, screenHeight, "raylibapp");
  SetTargetFPS(60);

  rlImGuiSetup(true);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLANK);

    DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);

    rlImGuiBegin();
    ImGui::Button("click me");
    bool open = true;
    ImGui::ShowDemoWindow(&open);
    rlImGuiEnd();

    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow();

  return 0;
}

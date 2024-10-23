#include "ui.hpp"
#include "imgui.h"
#include "stats.hpp"

void draw_ui(Game &game, Statistics &stats, float dt) {
  rlImGuiBegin();
  auto b = ImGui::Button("click me");

  ImGui::Text("Entities: %zu", game.ecs.view<entt::entity>().size_hint());

  ImGui::Text("Physics bodies: %i", stats.b2counters.bodyCount);
  ImGui::Text("Delta time: %6.3f ms", dt * 1000.0f);
  ImGui::Text("Physics time: %6.3f ms",
              static_cast<float>(stats.physics.count()) / 1000000.0f);
  ImGui::Text("Color time: %6.3f ms",
              static_cast<float>(stats.color.count()) / 1000000.0f);
  ImGui::Text("Draw time: %6.3f ms",
              static_cast<float>(stats.draw.count()) / 1000000.0f);

  ImGui::Checkbox("Enable rendering ", &game.enable_render);
  ImGui::Checkbox("Enable physics ", &game.enable_physics);

  if (b) {
    for (int i = 0; i < 500; i++) {
      spawn_new_body(game.ecs, game.physicsId);
    }
  }
  rlImGuiEnd();
}

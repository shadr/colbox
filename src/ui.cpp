#include "ui.hpp"
#include "game.hpp"
#include "imgui.h"
#include "stats.hpp"
#include "tool.hpp"

void Ui::draw_ui(Statistics &stats, float dt) {
  rlImGuiBegin();

  auto spawn_new_shapes = ImGui::Button("Spawn shapes");
  ImGui::SameLine();
  ImGui::InputInt("Amount", &spawn_amount);

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

  bool is_gravity_changed =
      ImGui::SliderFloat2("Gravity", game.gravity, -20.0f, 20.0f);

  if (is_gravity_changed)
    game.update_gravity();

  static const char *tool_names[] = {"None", "Force", "Paint"};
  bool is_tool_changed = ImGui::Combo("Tool", &tool_index, tool_names, 3);

  if (is_tool_changed) {
    switch (tool_index) {
    case 0:
      game.current_tool = new NoneTool();
      break;
    case 1:
      game.current_tool = new ForceTool();
      break;
    case 2:
      game.current_tool = new PaintPropertyTool();
      break;
    }
  }

  game.current_tool->ui();

  if (spawn_new_shapes) {
    for (int i = 0; i < spawn_amount; i++) {
      spawn_new_body(game.ecs, game.physicsId);
    }
  }
  rlImGuiEnd();
}

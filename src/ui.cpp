#include "ui.hpp"
#include "game.hpp"
#include "imgui.h"
#include "stats.hpp"
#include "tool.hpp"

void Ui::draw_ui(Statistics &stats, float dt) {
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

  bool is_gravity_changed =
      ImGui::SliderFloat2("Gravity", game.gravity, -20.0f, 20.0f);

  if (is_gravity_changed)
    game.update_gravity();

  static const char *tool_names[] = {"None", "Force", "Paint"};
  bool is_tool_changed = ImGui::Combo("Tool", &tool_index, tool_names, 3);

  if (is_tool_changed) {
    switch (tool_index) {
    case 0:
      game.tool = Tool::None;
      break;
    case 1:
      game.tool = Tool::Force;
      break;
    case 2:
      game.tool = Tool::Paint;
      break;
    }
  }

  if (game.tool == Tool::Paint) {
    static const char *property_names[] = {
        "Friction",
        "Restitution",
    };
    if (ImGui::Combo("Property", &property_index, property_names, 2)) {
      switch (property_index) {
      case 0:
        game.paint_data.prop = PaintProperty::Friction;
        break;
      case 1:
        game.paint_data.prop = PaintProperty::Restitution;
        break;
      }
    }
    ImGui::SliderFloat("Property value", &game.paint_data.value, 0.0f, 1.0f);
  }

  if (b) {
    for (int i = 0; i < 500; i++) {
      spawn_new_body(game.ecs, game.physicsId);
    }
  }
  rlImGuiEnd();
}

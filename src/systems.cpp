#include "systems.hpp"
#include "components.hpp"
#include "pch.hpp"
#include "rlgl.h"
#include "tool.hpp"

void color_system(entt::registry &reg, BaseTool &current_tool) {
  const auto view = reg.view<PhysicsComponent, ColorComponent>();
  if (PaintPropertyTool *paint_tool =
          dynamic_cast<PaintPropertyTool *>(&current_tool)) {
    b2ShapeId arr[8]{};
    view.each([&arr, paint_tool](PhysicsComponent &p, ColorComponent &c) {
      auto property_value = paint_tool->property;
      if (b2Body_GetShapes(p.id, arr, 8)) {
        auto shape_id = arr[0];
        auto value = property_value->get_property(shape_id);
        const static glm::vec4 min_color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        const static glm::vec4 max_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        auto color = glm::mix(min_color, max_color, value) * 255.0f;
        auto rcolor = Color(color.x, color.y, color.z, color.w);
        auto hsv = ColorToHSV(rcolor);
        c.hue = hsv.x;
      }
    });
  } else {
    view.each([](PhysicsComponent &p, ColorComponent &c) {
      auto vel = b2Body_GetLinearVelocity(p.id);
      auto radians = std::atan2(vel.y, vel.x);
      auto degrees = radians * 180.0 / std::numbers::pi;
      c.hue = degrees;
    });
  }
}

void mouse_interaction_system(BaseTool &current_tool, b2WorldId physicsId) {
  current_tool.update(physicsId);
}

void remove_escaped_circles(entt::registry &reg) {
  const auto view = reg.view<PhysicsComponent>();
  const auto screen_width = GetScreenWidth();
  const auto screen_height = GetScreenHeight();
  view.each(
      [&reg, screen_width, screen_height](entt::entity e, PhysicsComponent &p) {
        auto pos = b2Body_GetPosition(p.id);
        if (pos.x < 0.0 || pos.x > screen_width || pos.y < 0.0 ||
            pos.y > screen_height) {
          b2DestroyBody(p.id);
          reg.destroy(e);
        }
      });
}

#include "systems.hpp"
#include "box2d/box2d.h"
#include "components.hpp"
#include "pch.hpp"
#include "rlgl.h"
#include "tool.hpp"

void color_system(entt::registry &reg, float) {
  const auto view = reg.view<PhysicsComponent, ColorComponent>();
  view.each([](PhysicsComponent &p, ColorComponent &c) {
    auto vel = b2Body_GetLinearVelocity(p.id);
    auto radians = std::atan2(vel.y, vel.x);
    auto degrees = radians * 180.0 / std::numbers::pi;
    c.hue = degrees;
  });
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

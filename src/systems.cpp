#include "systems.hpp"
#include "components.hpp"
#include "pch.hpp"

struct OverlapContext {
  b2Vec2 origin;
  float multiplier;
};

void color_system(entt::registry &reg, float) {
  const auto view = reg.view<PhysicsComponent, ColorComponent>();
  view.each([](PhysicsComponent &p, ColorComponent &c) {
    auto vel = b2Body_GetLinearVelocity(p.id);
    auto radians = std::atan2(vel.y, vel.x);
    auto degrees = radians * 180.0 / std::numbers::pi;
    c.hue = degrees;
  });
}

void draw_system(entt::registry &reg) {
  const auto view = reg.view<const PhysicsComponent, const ColorComponent>();
  view.each([](const PhysicsComponent &p, const ColorComponent &c) {
    Color color = ColorFromHSV(c.hue, 1.0, 1.0);
    auto pos = b2Body_GetPosition(p.id);
    DrawCircle(pos.x, pos.y, p.radius, color);
  });
}

bool overlapCallback(b2ShapeId shapeId, void *ctxptr) {
  OverlapContext context = *reinterpret_cast<OverlapContext *>(ctxptr);
  b2BodyId bodyId = b2Shape_GetBody(shapeId);
  auto position = b2Body_GetPosition(bodyId);
  auto distance_squared = b2DistanceSquared(position, context.origin);
  auto dir = b2Normalize(position - context.origin);
  auto force = dir * 2e10f;
  force.x /= distance_squared;
  force.y /= distance_squared;
  b2Body_ApplyForceToCenter(bodyId, force * context.multiplier, true);
  return true;
}

void mouse_interaction_system(b2WorldId physicsId) {
  const float force_radius = 300.0;
  float force_direction = 0.0f;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    force_direction += 1.0f;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    force_direction -= 1.0f;
  }
  if (force_direction != 0.0f) {
    auto mouse_pos = GetMousePosition();
    b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
    b2Circle circle = {origin, force_radius};
    OverlapContext context{origin, force_direction};
    b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                          b2DefaultQueryFilter(), &overlapCallback, &context);
  }
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

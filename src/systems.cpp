#include "systems.hpp"
#include "box2d/box2d.h"
#include "components.hpp"
#include "pch.hpp"
#include "rlgl.h"
#include "tool.hpp"

struct ForceToolContext {
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

bool force_tool_callback(b2ShapeId shapeId, void *ctxptr) {
  ForceToolContext context = *reinterpret_cast<ForceToolContext *>(ctxptr);
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

void apply_force_at_mouse_position(b2WorldId physicsId, float tool_radius) {
  float force_direction = (float)IsMouseButtonDown(MOUSE_BUTTON_LEFT) -
                          (float)IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
  if (force_direction != 0.0f) {
    auto mouse_pos = GetMousePosition();
    b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
    b2Circle circle = {origin, tool_radius};
    ForceToolContext context{origin, force_direction};
    b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                          b2DefaultQueryFilter(), &force_tool_callback,
                          &context);
  }
}

bool paint_tool_callback(b2ShapeId shapeId, void *ctxptr) {
  PaintData *paint_data = reinterpret_cast<PaintData *>(ctxptr);
  switch (paint_data->prop) {
  case PaintProperty::Restitution:
    b2Shape_SetRestitution(shapeId, paint_data->value);
    break;
  case PaintProperty::Friction:
    b2Shape_SetFriction(shapeId, paint_data->value);
    break;
  default:
    break;
  }
  return true;
}

void paint_objects(b2WorldId physicsId, float tool_radius,
                   PaintData paint_data) {
  if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    return;
  }
  auto mouse_pos = GetMousePosition();
  b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
  b2Circle circle = {origin, tool_radius};
  b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                        b2DefaultQueryFilter(), &paint_tool_callback,
                        &paint_data);
}

void mouse_interaction_system(b2WorldId physicsId, Tool tool, float tool_radius,
                              PaintData paint_data) {
  switch (tool) {
  case Tool::None:
    break;
  case Tool::Force:
    apply_force_at_mouse_position(physicsId, tool_radius);
    break;
  case Tool::Paint:
    paint_objects(physicsId, tool_radius, paint_data);
    break;
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

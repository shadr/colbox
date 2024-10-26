#include "tool.hpp"
#include "box2d/box2d.h"
#include "raylib.h"
#include "rlgl.h"

void ToolWithRadius::ui() {
  ImGui::SliderFloat("Tool radius", &radius, 1.0f, 1000.0f);
}

void ToolWithRadius::draw() {
  rlSetLineWidth(0.25f);
  DrawCircleLinesV(GetMousePosition(), radius, RAYWHITE);
}

struct ForceToolContext {
  b2Vec2 origin;
  float multiplier;
};

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

void ForceTool::update(b2WorldId physicsId) {
  float force_direction = (float)IsMouseButtonDown(MOUSE_BUTTON_LEFT) -
                          (float)IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
  if (force_direction != 0.0f) {
    auto mouse_pos = GetMousePosition();
    b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
    b2Circle circle = {origin, radius};
    ForceToolContext context{origin, force_direction};
    b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                          b2DefaultQueryFilter(), &force_tool_callback,
                          &context);
  }
}

bool paint_tool_callback(b2ShapeId shapeId, void *ctxptr) {
  BasePaintProperty *property_object =
      reinterpret_cast<BasePaintProperty *>(ctxptr);
  property_object->set_property(shapeId);
  return true;
}

void PaintPropertyTool::update(b2WorldId physicsId) {
  if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    return;
  }
  auto mouse_pos = GetMousePosition();
  b2Vec2 origin = {mouse_pos.x, mouse_pos.y};
  b2Circle circle = {origin, radius};
  b2World_OverlapCircle(physicsId, &circle, b2Transform_identity,
                        b2DefaultQueryFilter(), &paint_tool_callback, property);
}

void PaintPropertyTool::ui() {
  ToolWithRadius::ui();
  static const char *property_names[] = {
      "Friction",
      "Restitution",
  };
  if (ImGui::Combo("Property", &ui_property_index, property_names, 2)) {
    switch (ui_property_index) {
    case 0:
      property = new PaintFrictionProperty();
      break;
    case 1:
      property = new PaintRestitutionProperty();
      break;
    }
  }
  property->ui();
}

void PaintFrictionProperty::ui() {
  ImGui::SliderFloat("Friction", &value, 0.0f, 1.0f);
}

void PaintFrictionProperty::set_property(b2ShapeId shapeId) {
  b2Shape_SetFriction(shapeId, value);
}

void PaintRestitutionProperty::ui() {
  ImGui::SliderFloat("Restitution", &value, 0.0f, 1.0f);
}

void PaintRestitutionProperty::set_property(b2ShapeId shapeId) {
  b2Shape_SetRestitution(shapeId, value);
}

#pragma once
#include "pch.hpp"
#include "tool.hpp"

void color_system(entt::registry &reg, float);

bool force_tool_callback(b2ShapeId shapeId, void *ctxptr);

void mouse_interaction_system(b2WorldId physicsId, Tool tool,
                              float tool_radius, PaintData paint_data);

void remove_escaped_circles(entt::registry &reg);

#pragma once
#include "pch.hpp"

void color_system(entt::registry &reg, float);

void draw_system(entt::registry &reg);

bool overlapCallback(b2ShapeId shapeId, void *ctxptr);

void mouse_interaction_system(b2WorldId physicsId);

void remove_escaped_circles(entt::registry &reg);

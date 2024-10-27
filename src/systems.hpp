#pragma once
#include "pch.hpp"
#include "tool.hpp"

void color_system(entt::registry &reg, BaseTool &current_tool);

void mouse_interaction_system(BaseTool &current_tool, b2WorldId physicsId
                              );

void remove_escaped_circles(entt::registry &reg);

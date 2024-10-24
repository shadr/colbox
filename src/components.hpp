#pragma once
#include "pch.hpp"

struct PhysicsComponent {
  b2BodyId id;
  float radius;

  PhysicsComponent(b2BodyId id, float r);
};

struct ColorComponent {
  float hue;
  ColorComponent();
};

struct WallMarker {
  b2BodyId id;
};

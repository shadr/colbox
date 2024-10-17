#include "components.hpp"
#include "random_ranges.hpp"

PhysicsComponent::PhysicsComponent(b2BodyId id, float r) : id(id), radius(r) {}

ColorComponent::ColorComponent() : hue(dis_start_color(rng)) {}

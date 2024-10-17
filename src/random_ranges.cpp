#include "random_ranges.hpp"

std::mt19937 rng =
    std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> dis_position(-300.0, 300.0);
std::uniform_real_distribution<float> dis_size(2.0, 2.0);
std::uniform_real_distribution<float> dis_vel_angle(0.0, 2 * std::numbers::pi);
std::uniform_real_distribution<float> dis_start_color(0.0, 360.0);

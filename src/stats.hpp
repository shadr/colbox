#pragma once
#include "pch.hpp"

struct Statistics {
  std::chrono::duration<long, std::ratio<1, 1000000000>> draw;
  std::chrono::duration<long, std::ratio<1, 1000000000>> color;
  std::chrono::duration<long, std::ratio<1, 1000000000>> physics;
  b2Counters &b2counters;
};

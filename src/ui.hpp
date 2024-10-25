#pragma once
#include "stats.hpp"

struct Game;

class Ui {
  Game &game;
  int tool_index = 1;
  int property_index = 0;
  int spawn_amount = 500;

public:
  Ui(Game &game) : game(game) {}

  void draw_ui(Statistics &stats, float dt);
};

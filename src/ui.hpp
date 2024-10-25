#pragma once
#include "stats.hpp"

struct Game;

class Ui {
  Game &game;
  int tool_index = 1;

public:
  Ui(Game &game) : game(game) {}

  void draw_ui(Statistics &stats, float dt);
};

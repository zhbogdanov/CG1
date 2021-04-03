#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

  bool Moved() const;
  void ProcessInput(MovementDir dir, int **stop, Image &character, bool &death, bool *next_level, bool &exit_game, bool &get_coin, Point &coin_coords, bool &change_character);
  void Draw(Image &screen, Image &img, Image &character, float alpha);
  void PutCoords(int x, int y) {coords.x = x; coords.y = y;}
  Point GetCoords() {return coords;}
  int GetSpeed() {return move_speed;}

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 255, .a = 255};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H

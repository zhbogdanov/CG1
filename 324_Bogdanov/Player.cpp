#include "Player.h"

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

void Player::ProcessInput(MovementDir dir, int **stop, Image &character, bool &death, bool *next_level, bool &exit_game, bool &get_coin, Point &coin_coords, bool &change_character)
{
  int move_dist = move_speed * 1;
  int check_coords, check_x_or_y;
  bool move = true;
  bool live1, live2, live3, live4;
  live1 = live2 = live3 = live4 = true;
  switch(dir)
  {
    case MovementDir::UP:
      old_coords.y = coords.y;
      
      for (int i = coords.x; i < coords.x + character.Width(); ++i) 
      {
        check_x_or_y = coords.y + 1 + character.Height();
        check_coords = stop[i][check_x_or_y];
        if (check_coords == 1) 
        {
          move = false;
          change_character = false;
          break;
        } else if (check_coords == 2) 
        {
          live1 = false;
          break;
        } else if ((check_coords == 3) && (1024 - (check_x_or_y) < 4)) 
        {
          next_level[0] = true;
          break;
        } else if ((check_coords == 4) && (1024 - (check_x_or_y) < 4)) 
        {
          exit_game = true;
          break;
        } else if (check_coords == 6) 
        {
          get_coin = true;
          coin_coords.x = i;
          coin_coords.y = check_x_or_y;
          break;
        }
      }
      if (move && !exit_game)
        coords.y += move_dist;
      move = true;
      break;

    case MovementDir::DOWN:
      old_coords.y = coords.y;
      for (int i = coords.x; i < coords.x + character.Width(); ++i) 
      {
        check_x_or_y = coords.y - 1;
        check_coords = stop[i][check_x_or_y];
        if (check_coords == 1) 
        {
          move = false;
          change_character = false;
          break;
        } else if (check_coords == 2) 
        {
          live2 = false;
          break;
        } else if ((check_coords == 3) && (check_x_or_y < 4)) 
        {
          next_level[1] = true;
          break;
        } else if ((check_coords == 4) && (check_x_or_y < 4)) 
        {
          exit_game = true;
          break;
        } else if (check_coords == 6) 
        {
          get_coin = true;
          coin_coords.x = i;
          coin_coords.y = check_x_or_y;
          break;
        }
      }
      if (move && !exit_game)
        coords.y -= move_dist;
      move = true;
      break;

    case MovementDir::LEFT:
      old_coords.x = coords.x;
      
      for (int j = coords.y; j < coords.y + character.Height(); ++j) 
      {
        check_x_or_y = coords.x - 1;
        check_coords = stop[check_x_or_y][j];
        //std::cout << check_x_or_y << std::endl;
        if (check_coords == 1) 
        {
          move = false;
          change_character = false;
          break;
        } else if (check_coords == 2) 
        {
          live3 = false;
          break;
        } else if ((check_coords == 3) && (check_x_or_y < 4)) 
        {
          next_level[2] = true;
          break;
        } else if ((check_coords == 4) && (check_x_or_y < 4)) 
        {
          exit_game = true;
          break;
        } else if (check_coords == 6) 
        {
          get_coin = true;
          coin_coords.x = check_x_or_y;
          coin_coords.y = j;
          break;
        }
      }
      if (move && !exit_game)
        coords.x -= move_dist;
      move = true;
      break;

    case MovementDir::RIGHT:
      old_coords.x = coords.x;
      for (int j = coords.y; j < coords.y + character.Height(); ++j) 
      {
        check_x_or_y = coords.x + 1 + character.Width();
        check_coords = stop[check_x_or_y][j];
        if (check_coords == 1) 
        {
          move = false;
          change_character = false;
          break;
        } else if (check_coords == 2) 
        {
          live4 = false;
          break;
        } else if ((check_coords == 3) && (1024 - (check_x_or_y) < 6)) 
        {
          next_level[3] = true;
          break;
        } else if ((check_coords == 4) && (1024 - (check_x_or_y) < 6)) 
        {
          exit_game = true;
          break;
        } else if (check_coords == 6) 
        {
          get_coin = true;
          coin_coords.x = check_x_or_y;
          coin_coords.y = j;
          break;
        }
      }
      if (move && !exit_game)
        coords.x += move_dist;
      move = true;
      break;
    default:
      break;
  }
  death = !live1 || !live2 || !live3 || !live4;
}

void Player::Draw(Image &screen, Image &img, Image &character, float alpha)
{
  Pixel pixel_buffer;
  if(Moved())
  {
    for(int y = old_coords.y; y < old_coords.y + character.Height(); ++y)
    {
      for(int x = old_coords.x; x < old_coords.x + character.Width(); ++x)
      {
          pixel_buffer = img.GetPixel(x, y);
          pixel_buffer = {static_cast<uint8_t>(pixel_buffer.r * alpha),
                          static_cast<uint8_t>(pixel_buffer.g * alpha),
                          static_cast<uint8_t>(pixel_buffer.b * alpha),
                          static_cast<uint8_t>(pixel_buffer.a * alpha)};
          screen.PutPixel(x, y, pixel_buffer);
      }
    }
    old_coords = coords;
  }

  for(int y = coords.y; y < coords.y + character.Height(); ++y)
  {
    for(int x = coords.x; x < coords.x + character.Width(); ++x)
    {
        Pixel check = character.GetPixel(x - coords.x, character.Height() - y + coords.y - 1);
        if ((check.r + check.g + check.b + check.a) > 0) 
        {
          check = {static_cast<uint8_t>(check.r * alpha),
                          static_cast<uint8_t>(check.g * alpha),
                          static_cast<uint8_t>(check.b * alpha),
                          static_cast<uint8_t>(check.a * alpha)};
          screen.PutPixel(x, y, check);
        
        }
    }                                                                                                                                                               
  }
}
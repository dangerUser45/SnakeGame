#pragma once

#include <deque>
#include <functional>

#include "coord.hpp"
#include "decor.hpp"
namespace snake_game {

class Snake  {
public:
    Direction dir_;
    std::deque<Coord> body_{};
    std::function<Direction()> move_algorythm_{};
    ObjColor color_{};

    Snake(Direction dir = Direction::UP);
    
    Direction GetRandomDirection();
    void ChangeDir(Direction dir);
    void Move();
};

} //namespace snake_game

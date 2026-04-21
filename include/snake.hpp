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
    Coord prev_tail_{};
    bool is_live_ = true;

    Snake(Direction dir = Direction::UP);
    
    void ChangeDir(Direction dir);
    void Move();
};

} //namespace snake_game

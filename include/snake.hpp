#pragma once

#include <deque>

#include "coord.hpp"
namespace snake_game {

class Snake final {
public:
    int8_t color_;
    Direction dir_;
    std::deque<Coord> body_;

    Snake(Direction dir = Direction::UP);
    
    void ChangeDir(Direction dir);
    void Move();

    bool IsRotate180Degrees(Direction dir) const;
};

} //namespace snake_game

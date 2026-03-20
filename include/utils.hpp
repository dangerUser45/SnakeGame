#pragma once

#include <cstdint>

namespace snake_game {

enum class Direction { LEFT, RIGHT, UP, DOWN };
    
struct Coord {
    int32_t x,y;
    
    Coord& operator+=(Direction dir) {
        switch (dir) {
            case Direction::UP:    --y; break;
            case Direction::DOWN:  ++y; break;
            case Direction::RIGHT: ++x; break;
            case Direction::LEFT:  --x; break;
        }
        return *this;
    }

    inline Coord operator+(const Direction& dir) {
        Coord new_coord = *this;
        new_coord += dir;
        return new_coord;
    }
};

} // namespace snake_game

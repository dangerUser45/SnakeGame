#pragma once

#include <iostream>

namespace snake_game {

enum class Direction { UNKNOWN, LEFT, RIGHT, UP, DOWN };

struct Coord {
    int x,y;

    bool operator ==(Coord& second) const {
        if(this->x == second.x && this->y == second.y)
            return true;
        else return false;
    }
    
    Coord& operator+=(Direction dir) {
        switch (dir) {
            case Direction::UP:    --y; break;
            case Direction::DOWN:  ++y; break;
            case Direction::RIGHT: ++x; break;
            case Direction::LEFT:  --x; break;

            default: std::cerr << "Error: uknown directions" << std::endl;
                return *this;
        }
        return *this;
    }

    inline Coord operator+(const Direction& dir) {
        Coord new_coord = *this;
        new_coord += dir;
        return new_coord;
    }
};

Coord GetRandomCoord(Coord borders);
Direction GetRandomDirection();

} // namespace snake_game

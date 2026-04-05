#pragma once

#include <iostream>
#include <random>

namespace snake_game {

enum class Direction { UNKNOWN, LEFT, RIGHT, UP, DOWN };

struct Coord {
    int x,y;

    bool operator==(const Coord& second) const {
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

    inline Coord operator-(const Direction& dir) {
        Coord new_coord = *this;
        switch (dir) {
            case Direction::UP:    new_coord += Direction::DOWN;  break;
            case Direction::DOWN:  new_coord += Direction::UP;    break;
            case Direction::RIGHT: new_coord += Direction::LEFT;  break;
            case Direction::LEFT:  new_coord += Direction::RIGHT; break;

            default: std::cerr << "Error: uknown directions" << std::endl;
                return new_coord;
        }
        return new_coord;
    }
};

Coord GetRandomCoord(Coord left_top_corner, Coord rigth_bottom_corner,  std::mt19937& gen);
Direction GetRandomDirection(std::mt19937& gen);

std::ostream& operator<< (std::ostream& os, const Coord& coord);

} // namespace snake_game

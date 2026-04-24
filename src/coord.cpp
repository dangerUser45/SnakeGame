#include <random>

#include "coord.hpp"

namespace snake_game {

Coord GetRandomCoord(Coord left_top_corner, Coord right_bottom_corner, std::mt19937& gen)
{
    std::uniform_int_distribution<int> dist_x(left_top_corner.x, right_bottom_corner.x);
    std::uniform_int_distribution<int> dist_y(left_top_corner.y, right_bottom_corner.y);

    return {dist_x(gen), dist_y(gen)};
}

Direction GetRandomDirection(std::mt19937& gen)
{
    std::uniform_int_distribution<int> number(0, 3);

    switch(number(gen)) {
        case 0: return Direction::UP;
        case 1: return Direction::DOWN;
        case 2: return Direction::LEFT;
        case 3: return Direction::RIGHT;

        default: std::cerr << "Error: Undefined direction" << std::endl;
            return Direction::UNDEFINED;
    }
}

std::ostream& operator<< (std::ostream& os, const Coord& coord)
{
    return os << "{" << coord.x << ", " << coord.y << "}";
}

} // namespace snake_game

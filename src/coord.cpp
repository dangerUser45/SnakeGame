#include <random>

#include "coord.hpp"

namespace snake_game {

Coord GetRandomCoord(Coord border)
{
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<int> dist_x(1, border.x - 2);
    std::uniform_int_distribution<int> dist_y(1, border.y - 2);

    return {dist_x(gen), dist_y(gen)};
}

Direction GetRandomDirection()
{
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<int> number(0, 3);

    switch(number(gen)) {
        case 0: return Direction::UP;
        case 1: return Direction::DOWN;
        case 2: return Direction::LEFT;
        case 3: return Direction::RIGHT;

        default: std::cerr << "Error: Unknown direction" << std::endl;
            return Direction::UNKNOWN;
    }
}

} // namespace snake_game

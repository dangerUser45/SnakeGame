#include "coord.hpp"

namespace snake_game {

Coord GetRandomCoord(Coord border)
{
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<int32_t> dist_x(1, border.x - 2);
    std::uniform_int_distribution<int32_t> dist_y(1, border.y - 2);

    return {dist_x(gen), dist_y(gen)};
}

} // namespace snake_game
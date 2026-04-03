#pragma once

#include "coord.hpp"

namespace snake_game {

class Rabbit final {
public:
    Coord body_{};
    Rabbit(Coord coord);
};

} // namespace snake_game

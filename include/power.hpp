#pragma once

#include "coord.hpp"

namespace snake_game {

constexpr int ABILITY_TIME = 60; // ticks

struct Power final {
    Coord body_;
};

} // namespace snake_game

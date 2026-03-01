#pragma once

#include <cstdint>
#include <list>
#include <utility>

enum class Direction { LEFT, RIGHT, UP, DOWN };

namespace snake_game {

class Body {
public:


private:
    uint8_t color;
    Direction dir;
    std::list<std::pair<int, int>> body;
};

} // namespace snake_game

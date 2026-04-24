#pragma once

#include <deque>

#include "coord.hpp"
#include "decor.hpp"

namespace snake_game {

enum class BotAlgorithm : std::size_t {DUMB, MEDIUM, SMARTY, NOT_BOT};

class Snake  {
public:
    Direction dir_;
    std::deque<Coord> body_{};
    ObjColor color_{};
    Coord prev_tail_{};
    BotAlgorithm bot_{BotAlgorithm::SMARTY};
    int kill_num = 0;
    bool is_live_ = true;
    bool is_power_active = false;
    int power_ticks_left = 0;

    Snake(Direction dir = Direction::UP);
    
    void ChangeDir(Direction dir);
    void Move();
};

} //namespace snake_game

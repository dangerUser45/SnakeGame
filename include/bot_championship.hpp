#pragma once

#include <string>
#include <vector>

#include "decor.hpp"
#include "snake.hpp"

namespace snake_game {

struct BotChampionshipEntry final {
    BotAlgorithm algorithm_ = BotAlgorithm::DUMB;
    ObjColor color_ = ObjColor::WITHOUT_COLOR;
    std::string label_{};
    int wins_ = 0;
    int survived_rounds_ = 0;
    int total_kills_ = 0;
};

struct BotChampionshipStats final {
    int rounds_ = 0;
    int draws_ = 0;
    int timeouts_ = 0;
    int max_ticks_per_round_ = 0;
    std::vector<BotChampionshipEntry> entries_{};
};

} // namespace snake_game

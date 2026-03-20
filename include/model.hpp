#pragma once

#include <chrono>
#include <cstdint>
#include <unistd.h>
#include <vector>

#include "rabbit.hpp"
#include "snake.hpp"
#include "coord.hpp"

namespace snake_game {

class Model
{
public:
    enum class PlayersMode { SINGLE_PLAYER, TWO_PLAYER };

    Model(Coord win_size = { DEFAULT_WIDTH, DEFAULT_HEIGTH },
          int32_t num_snakes = DEFAULT_NUM_SNAKES,
          int32_t num_rabbits = DEFAULT_NUM_RABBITS,
          PlayersMode players_mode = PlayersMode::SINGLE_PLAYER  
         );

    void Update();

    static const int32_t DEFAULT_WIDTH = 145;
    static const int32_t DEFAULT_HEIGTH = 34;
    static const int32_t DEFAULT_NUM_RABBITS = 3;
    static const int32_t DEFAULT_NUM_SNAKES = 1;
    struct Updates
    {
        // color, position
    };

    Coord win_size_;

    std::vector<Snake> snakes_;
    std::vector<Rabbit> rabbits_;
    PlayersMode players_mode_;

    bool is_single_player() const noexcept;

    // std::vector<Update> updates_; //TODO добавить изменения

    std::chrono::milliseconds tic_time_{50};

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

struct Model::Impl final
{
public:
    Impl();
    void SnakeAllocator(Snake& snake);
private:

};

} // namespace snake_game

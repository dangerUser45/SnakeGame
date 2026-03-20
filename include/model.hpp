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
          int32_t num_rabbits = DEFAULT_NUM_RABBITS,
          PlayersMode players_mode = PlayersMode::SINGLE_PLAYER  
         );

    void Update();

     struct Updates
    {
        // color, position
    };

    Coord win_size_;

    std::vector<Snake> snakes_;
    std::vector<Rabbit> rabbits_;
    PlayersMode players_mode_;

    bool is_single_player() const noexcept;
    void SnakeAllocator(Snake& snake);

    struct Builder {

        Coord win_size_ = { DEFAULT_WIDTH, DEFAULT_HEIGTH }; 

        std::vector<Snake> snakes_;
        std::vector<Rabbit> rabbits_;
        PlayersMode players_mode_ = PlayersMode::SINGLE_PLAYER;

        Builder() = default;

        Builder& SetWinSize(Coord win_size) 
        {
            win_size_ = win_size;
            return *this; 
        }

        Builder& SetNumRabbits(int32_t num_rabbits)
        {
            (void)num_rabbits;
            // num_rabbits_ = num_rabbits;
            return *this;
        }

        Builder& SetPlayersMode(PlayersMode players_mode)
        {
            players_mode_ = players_mode;
            return *this;
        }

        Model Build() const
        {
            return Model();
        }
    };

    // std::vector<Update> updates_; //TODO добавить изменения

    std::chrono::milliseconds tic_time_{100};

private:
    static const int32_t DEFAULT_WIDTH       = 145;
    static const int32_t DEFAULT_HEIGTH      = 34;
    static const int32_t DEFAULT_NUM_RABBITS = 3;
    static const int32_t DEFAULT_NUM_SNAKES  = 1;
};

} // namespace snake_game

#pragma once

#include <chrono>
#include <cstdint>
#include <unistd.h>
#include <vector>
#include <list>

#include "rabbit.hpp"
#include "snake.hpp"
#include "coord.hpp"

namespace snake_game {

class Model
{
public:
    enum class PlayersMode { SINGLE_PLAYER, TWO_PLAYER };

    Model(Coord win_size, PlayersMode players_mode);

    void Update();

     struct Updates
    {
        // color, position
    };

    Coord win_size_;
    PlayersMode players_mode_;

    std::list<Snake> snakes_;
    std::vector<Rabbit> rabbits_;
    std::vector<Snake*> hcontrol_;
    std::chrono::milliseconds tic_time_{50};
    std::size_t rabbits_per_snake_ = 6; // TODO добавить билдер для этого поля

    struct Builder;

    bool IsSinglePlayer() const noexcept;
    
private:
    void SpawnFirstPlayerSnake(Snake& snake);
    void SpawnSecondPlayerSnake(Snake& snake);
    void SnakesUpdate();
    bool SnakesOverlapped(Coord coord) const;
    bool RabbitsOverlapped(Coord coord, std::vector<Rabbit>::const_iterator& rabbit_iter) const;
    void BoundariesTeleportation(Snake& snake, Coord coord);
    void EatingRabbits(Snake& snake, Coord coord);
    bool Crashes(std::list<Snake>::iterator& it, Coord new_head_coord);
    void ZeroizeHContrSnake(std::list<Snake>::iterator it);

    static const int32_t DEFAULT_WIDTH       = 145;
    static const int32_t DEFAULT_HEIGTH      = 34;
    static const int32_t DEFAULT_NUM_RABBITS = 3;
};

struct Model::Builder {

    Coord win_size_ = { DEFAULT_WIDTH, DEFAULT_HEIGTH };
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
        return Model(win_size_, players_mode_);
    }
};

} // namespace snake_game

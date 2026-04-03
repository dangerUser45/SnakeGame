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

Direction RotateDir(Direction dir);

class Model
{
public:
    enum class PlayersMode : std::uint8_t { SINGLE_PLAYER, TWO_PLAYERS, UNDEFINED };

    static constexpr const int   UNDEFINED_NUM   = -1;
    static constexpr const Coord UNDEFINED_COORD = { -1, -1 };

    Model(Coord win_size,
          int num_bots,
          int rabb_per_snake,
          PlayersMode player_mode);

    void Update();

    struct Updates
    {
        // color, position
    };

    Coord win_size_;
    PlayersMode players_mode_;

    std::list<Snake> snakes_{};
    std::vector<Rabbit> rabbits_{};
    std::vector<Snake*> hcontrol_{};
    std::chrono::milliseconds tic_time_{50};
    int rabb_per_snake_;

    struct Builder;

    bool IsSinglePlayer() const noexcept;
    
private:
    void SnakesUpdate();
    
    void SpawnNewSnake(Snake& snake);
    void SpawnFirstPlayerSnake(Snake& snake);
    void SpawnSecondPlayerSnake(Snake& snake);
    
    void InsertSnake(Snake& snake, bool& is_located, Coord second_part, Coord third_part);
    void GenerateRabbits();

    bool SnakesOverlapped(Coord coord) const;
    bool RabbitsOverlapped(Coord coord, std::vector<Rabbit>::const_iterator& rabbit_iter) const;
    
    bool Crashes(std::list<Snake>::iterator& it, Coord new_head_coord);
    void ZeroizeHContrSnake(std::list<Snake>::iterator it);
    void EatingRabbits(Snake& snake, Coord coord);
    void BoundariesTeleportation(Snake& snake, Coord coord);
    void FillSnakesColor();

    static constexpr const int DEFAULT_WIDTH           = 145;
    static constexpr const int DEFAULT_HEIGTH          = 34;
    static constexpr const int DEFAULT_NUM_BOTS        = 2;
    static constexpr const int DEFAULT_RABB_PER_SNAKES = 5;
};

struct Model::Builder {

    Coord win_size_           = { DEFAULT_WIDTH, DEFAULT_HEIGTH };
    int num_bots_             = DEFAULT_NUM_BOTS;
    int rabb_per_snake_       = DEFAULT_RABB_PER_SNAKES;
    PlayersMode players_mode_ = PlayersMode::SINGLE_PLAYER;

    Builder() = default;

    Builder& SetWinSize(Coord win_size);
    Builder& SetNumBots(int num_bots);
    Builder& SetRabbPerSnake(int rabb_per_snake);
    Builder& SetPlayersMode(PlayersMode players_mode);
    Model Build() const;
};

} // namespace snake_game

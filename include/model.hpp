#pragma once

#include <chrono>
#include <random>
#include <unistd.h>
#include <utility>
#include <vector>
#include <list>

#include "decor.hpp"
#include "rabbit.hpp"
#include "snake.hpp"
#include "coord.hpp"

namespace snake_game {

Direction RotateDir(Direction dir);

class Model
{
public:
    static constexpr const int   UNDEFINED_NUM   = -1;

    Model(Coord win_size,
          int num_players,
          int num_bots,
          int tic_time,
          int rabb_per_snake);

    void Update();
    bool IsGameOver();

    enum class UpdKind { EMPTY, SNAKE_BODY, SNAKE_HEAD, RABBIT };
    struct Updates {
        Coord coord_;
        ObjColor color_;
        UpdKind upd_kind_;
        Direction dir_;
    };

    Coord win_size_;

    using SnakeIterT = std::list<Snake>::iterator;

    std::list<Snake> snakes_{};
    std::vector<Rabbit> rabbits_{};
    std::vector<Snake*> hcontrol_{};
    std::vector<Updates> updates_{};
    std::chrono::milliseconds tic_time_{};
    int num_players_;
    int num_bots_;
    int rabb_per_snake_;

    std::mt19937 gen_{std::random_device{}()};

    struct Builder;
    
private:
    void SnakesUpdate();
    void SpawnNewSnake(Snake& snake, int* counter);
    void InsertSnake(Snake& snake,
                     Coord head, Coord second_part, Coord third_part);
    void GenerateRabbits();
    [[nodiscard]] std::pair<Coord, Coord> GetSector(int snake_num);

    [[nodiscard]] bool SnakesOverlapped(Coord coord) const;
    [[nodiscard]] bool SnakesOverlapped(Coord coord, Updates& updates) const;
    [[nodiscard]] bool SnakesOverlapped(Coord coord, std::list<Snake>::iterator& current_snake) const;
    [[nodiscard]] bool RabbitsOverlapped(Coord coord, std::vector<Rabbit>::const_iterator& rabbit_iter) const;
    [[nodiscard]] bool RabbitsOverlapped(Coord coord) const;

    void MoveSnakes();
    void RemoveDeadSnakes();
    
    void Crashes(std::list<Snake>::iterator& it, Coord new_head_coord);
    void ZeroizeHContrSnake(std::list<Snake>::iterator it);
    void EatingRabbits(Snake& snake, Coord coord);
    void BoundariesTeleportation(Snake& snake, Coord coord);
    void FillSnakesColor();
    void ClearOldUpdates();

    bool game_over_ = false;

    static constexpr const int DEFAULT_WIDTH           = 145;
    static constexpr const int DEFAULT_HEIGTH          = 34;
    static constexpr const int DEFAULT_NUM_BOTS        = 2;
    static constexpr const int DEFAULT_RABB_PER_SNAKES = 5;
    static constexpr const int DEFAULT_NUM_PLAYERS     = 1;
    static constexpr const int DEFAULT_TIC_TIME        = 100; // milliseconds
};

struct Model::Builder {

    Coord win_size_           = { DEFAULT_WIDTH, DEFAULT_HEIGTH };
    int num_bots_             =   DEFAULT_NUM_BOTS;
    int rabb_per_snake_       =   DEFAULT_RABB_PER_SNAKES;
    int num_players_          =   DEFAULT_NUM_PLAYERS;
    int tic_time_             =   DEFAULT_TIC_TIME; 

    Builder() = default;

    [[nodiscard]] Builder& SetWinSize(Coord win_size);
    [[nodiscard]] Builder& SetNumBots(int num_bots);
    [[nodiscard]] Builder& SetTicTime(int tic_time);
    [[nodiscard]] Builder& SetRabbPerSnake(int rabb_per_snake);
    [[nodiscard]] Builder& SetNumPlayers(int num_players);
    [[nodiscard]] Model Build() const;
};

} // namespace snake_game

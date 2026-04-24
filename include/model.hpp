#pragma once

#include <chrono>
#include <list>
#include <random>
#include <unistd.h>
#include <utility>
#include <vector>

#include "coord.hpp"
#include "decor.hpp"
#include "rabbit.hpp"
#include "snake.hpp"
#include "power.hpp"

namespace snake_game {

enum class ViewMode {TERMINAL_VIEW, GRAPHICAL_VIEW, UNDEFINED_VIEW};

class Model
{
public:
    static constexpr const int UNDEFINED_NUM   = -1;

    Model(Coord win_size,
          ViewMode view_mode,
          int num_players,
          int num_bots,
          int tic_time,
          int rabb_per_snake);

    void Update();
    bool IsGameOver();

    enum class UpdKind { EMPTY, SNAKE_BODY, SNAKE_HEAD, RABBIT, POWER };
    struct Updates {
        Coord coord_;
        ObjColor color_;
        UpdKind upd_kind_;
        Direction dir_;
    };

    Coord win_size_;

    std::list<Snake> snakes_{};
    std::vector<Rabbit> rabbits_{};
    std::vector<Power> powers_{};
    std::vector<Snake*> hcontrol_{};
    std::vector<Updates> updates_{};
    std::chrono::milliseconds tic_time_{};
    int num_players_;
    int num_bots_;
    int rabb_per_snake_;
    ViewMode view_mode_;

    std::mt19937 gen_{std::random_device{}()};
    
    int power_spawn_counter_ = 0;
    int power_spawn_capacity_{};

    struct Builder;
    
private:

    void SnakesUpdate();
    void SpawnNewSnake(Snake& snake, int& counter);
    void InsertSnake(Snake& snake,
                     Coord head, Coord second_part, Coord third_part);
    void GenerateRabbits();
    void GeneratePowers();
    [[nodiscard]] bool IsSpawnPower();

    [[nodiscard]] std::pair<Coord, Coord> GetSector(int snake_num);

    void ActivatingPower(Snake& snake, Coord new_head_coord);

    [[nodiscard]] bool SnakesOverlapped(Coord coord) const;
    [[nodiscard]] bool SnakesOverlapped(Coord coord, Updates& updates) const;
    [[nodiscard]] bool SnakesOverlapped(Coord coord, std::list<Snake>::iterator& current_snake) const;
    [[nodiscard]] bool RabbitsOverlapped(Coord coord, std::vector<Rabbit>::const_iterator& rabbit_iter) const;
    [[nodiscard]] bool RabbitsOverlapped(Coord coord) const;
    [[nodiscard]] bool PowersOverlapped(Coord coord) const;
    [[nodiscard]] bool PowersOverlapped(Coord coord, std::vector<Power>::const_iterator& iter) const;

    void SetSnakesBotAlorithms();

    struct RabbitDistance final {
        const Rabbit* rabbit;
        std::size_t distance;
    };

    std::unique_ptr<std::vector<Model::RabbitDistance>>
    GetRabbitCandidates(Coord head, std::size_t num_rabbits) const;

    [[nodiscard]] Direction BotAlgorithm(Snake& snake) const;
    [[nodiscard]] Direction DumbBot(Snake& snake) const;
    [[nodiscard]] Direction MediumBot(Snake& snake) const;
    [[nodiscard]] Direction SmartyBot(Snake& snake) const;

    void MoveSnakes();
    void RemoveDeadSnakes();

    void Crashes(std::list<Snake>::iterator& it, Coord new_head_coord);
    void ZeroizeHContrSnake(std::list<Snake>::iterator it);
    void EatingRabbits(Snake& snake, Coord coord);
    void BoundariesTeleportation(Snake& snake, Coord coord);
    void SetSnakesColor();
    void ClearOldUpdates();

    std::unique_ptr<std::vector<std::vector<bool>>> BuildBlockMap() const;
    std::unique_ptr<std::vector<std::vector<int>>> BuildDangerMap() const;

    bool game_over_ = false;

    static constexpr int DEFAULT_WIDTH           = 120;
    static constexpr int DEFAULT_HEIGTH          = 34;
    static constexpr ViewMode DEFAULT_VIEW_MODE  = ViewMode::TERMINAL_VIEW; 
    static constexpr int DEFAULT_NUM_BOTS        = 2;
    static constexpr int DEFAULT_RABB_PER_SNAKES = 5;
    static constexpr int DEFAULT_NUM_PLAYERS     = 1;
    static constexpr int DEFAULT_TIC_TIME        = 100; // milliseconds

};

struct Model::Builder {

    Coord win_size_           = { DEFAULT_WIDTH, DEFAULT_HEIGTH };
    ViewMode view_mode_       =   DEFAULT_VIEW_MODE;
    int num_bots_             =   DEFAULT_NUM_BOTS;
    int rabb_per_snake_       =   DEFAULT_RABB_PER_SNAKES;
    int num_players_          =   DEFAULT_NUM_PLAYERS;
    int tic_time_             =   DEFAULT_TIC_TIME; 

    Builder() = default;

    [[nodiscard]] Builder& SetWinSize(Coord win_size);
    [[nodiscard]] Builder& SetViewMode(ViewMode view_mode);
    [[nodiscard]] Builder& SetNumBots(int num_bots);
    [[nodiscard]] Builder& SetTicTime(int tic_time);
    [[nodiscard]] Builder& SetRabbPerSnake(int rabb_per_snake);
    [[nodiscard]] Builder& SetNumPlayers(int num_players);
    [[nodiscard]] Model Build() const;

};

} // namespace snake_game

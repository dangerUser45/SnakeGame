// #include <random>

#include "model.hpp"
#include "coord.hpp"

namespace snake_game {

Model::Model(Coord win_size, PlayersMode players_mode) :
    win_size_(win_size),
    players_mode_(players_mode)
{
    // TODO добавить кроликов
    // rabbits_.reserve(num_rabbits);

    snakes_.emplace_back();
    SpawnFirstPlayerSnake(snakes_[0]);

    if(!IsSinglePlayer()) {
        snakes_.emplace_back();
        SpawnSecondPlayerSnake(snakes_[1]);
    }
}

void Model::Update()
{
    snakes_[0].Move();
    if(!IsSinglePlayer()) snakes_[1].Move();
}

bool Model::IsSinglePlayer() const noexcept
{
    if(players_mode_ == PlayersMode::SINGLE_PLAYER)
        return true;
    return false;
}

void Model::SpawnFirstPlayerSnake(Snake& snake)
{
    snake.dir_ = Direction::DOWN;

    snake.body_.emplace_back(1,3); 
    snake.body_.emplace_back(1,2);
    snake.body_.emplace_back(1,1);
}

void Model::SpawnSecondPlayerSnake(Snake& snake)
{
    snake.dir_ = Direction::UP;

    snake.body_.emplace_back(10,1);
    snake.body_.emplace_back(10,2);
    snake.body_.emplace_back(10,3);
}

} // namespace snake_game
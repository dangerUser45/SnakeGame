// #include <random>

#include "model.hpp"
#include "coord.hpp"

namespace snake_game {

Model::Model(Coord win_size,
    int32_t num_rabbits,
    PlayersMode players_mode
    ) :
    win_size_(win_size),
    players_mode_(players_mode)
{
    rabbits_.reserve(num_rabbits);

    snakes_.emplace_back();
    SnakeAllocator(snakes_[0]);

    if(!is_single_player()) {
        snakes_.emplace_back();
        SnakeAllocator(snakes_[1]);
    }
}

void Model::Update()
{
    snakes_[0].Move();
    if(!is_single_player()) snakes_[1].Move();
}

bool Model::is_single_player() const noexcept
{
    if(players_mode_ == PlayersMode::SINGLE_PLAYER)
        return true;
    return false;
}

void Model::SnakeAllocator(Snake& snake)
{ 
    //TODO написать нормальный аллокатор по карте
    snake.body_.emplace_back(1,3); snake.dir_ = Direction::DOWN;
    snake.body_.emplace_back(1,2);
    snake.body_.emplace_back(1,1);

    if(!is_single_player()) {
        snake.body_.emplace_back(10,1); snake.dir_ = Direction::UP;
        snake.body_.emplace_back(10,2);
        snake.body_.emplace_back(10,3);
    }
}

} // namespace snake_game
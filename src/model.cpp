#include <iostream> // DEBUG

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
    SnakesUpdate();

    // Moving snakes
    snakes_[0].Move();
    if(!IsSinglePlayer()) snakes_[1].Move();

    // Generating and placement rabbits
    if(rabbits_.size() < rabbits_per_snake_ * snakes_.size()) {
        Coord rabbit_coord = GetRandomCoord(win_size_);
        if(!SnakesOverlapped(rabbit_coord)) {
            rabbits_.emplace_back(rabbit_coord);
        }
    }
}

bool Model::IsSinglePlayer() const noexcept
{
    if(players_mode_ == PlayersMode::SINGLE_PLAYER)
        return true;
    return false;
}

void Model::SpawnFirstPlayerSnake(Snake& snake)
{
    snake.dir_ = Direction::RIGHT;

    snake.body_.emplace_back(4,2);
    snake.body_.emplace_back(3,2);
    snake.body_.emplace_back(2,2); 
}

void Model::SpawnSecondPlayerSnake(Snake& snake)
{
    snake.dir_ = Direction::LEFT;

    snake.body_.emplace_back(win_size_.x - 5, win_size_.y - 3);
    snake.body_.emplace_back(win_size_.x - 4, win_size_.y - 3);
    snake.body_.emplace_back(win_size_.x - 3, win_size_.y - 3);
}

bool Model::SnakesOverlapped(Coord coord) const
{
    for(auto&& snake : snakes_)
        for(auto&& body_part : snake.body_)
            if(body_part == coord)
                return true;
            else continue;
    
    return false;
}

bool Model::RabbitsOverlapped(Coord coord, std::vector<Rabbit>::const_iterator& iter) const
{
    for(auto rabbit_iter = rabbits_.cbegin(), cend = rabbits_.cend(); rabbit_iter != cend; ++rabbit_iter)
        if(rabbit_iter->body_ == coord) {
            iter = rabbit_iter;
            return true;
        }
        else continue;
    
    return false;
}

void Model::Crashes(std::vector<Snake>::iterator it, Coord new_head_coord)
{
    if(SnakesOverlapped(new_head_coord)) {
        // TODO добавить какую-нибудь надпись по типу: "О нет, змейка №X умерла!"
        snakes_.erase(it);
        /* DEBUG */  std::cout << "\033[" << win_size_.y + 2 << ";" << 2 << "H" << "Змейки столкнулись" << std::flush;
    }
}

void Model::BoundariesTeleportation(Snake& snake, Coord coord)
{
    // Top border
    if(coord.y == 0) {
        snake.body_.pop_front();
        snake.body_.emplace_front(coord.x, win_size_.y - 1);
    }

    // Bottom border
    if(coord.y == win_size_.y - 1) {
        snake.body_.pop_front();
        snake.body_.emplace_front(coord.x, 0);
    }

    // Left border
    if(coord.x == 0) {
        snake.body_.pop_front();
        snake.body_.emplace_front(win_size_.x - 1, coord.y);
    }

    // Rignt border
    if(coord.x == win_size_.x - 1) {
        snake.body_.pop_front();
        snake.body_.emplace_front(0, coord.y);
    }
}

void Model::SnakesUpdate()
{
    for(auto  snake_it = snakes_.begin(), end = snakes_.end(); snake_it != end; ++snake_it) {
        Coord new_head_coord = snake_it->body_.front() + snake_it->dir_;

        Crashes(snake_it, new_head_coord);
        EatingRabbits(*snake_it, new_head_coord);
        BoundariesTeleportation(*snake_it, new_head_coord);
    }
}

void Model::EatingRabbits(Snake& snake, Coord new_head_coord)
{

    std::vector<Rabbit>::const_iterator iter;
    if(RabbitsOverlapped(new_head_coord, iter)) {
        rabbits_.erase(iter);
    
        // вставляем пустой элемент, так как он съестся при Move()
        snake.body_.emplace_back();
    }
}

} // namespace snake_game
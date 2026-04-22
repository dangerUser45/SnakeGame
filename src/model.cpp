#include <functional>
#include <utility>

#include "coord.hpp"
#include "decor.hpp"
#include "model.hpp"

namespace snake_game {

Model::Model(Coord win_size,
             ViewMode view_mode,
             int num_players,
             int num_bots,
             int tic_time,
             int rabb_per_snake) :
    win_size_(win_size),
    tic_time_(tic_time),
    num_players_(num_players),
    num_bots_(num_bots),
    rabb_per_snake_(rabb_per_snake),
    view_mode_(view_mode)
{
    if(!(num_players + num_bots))
        throw std::runtime_error("Error: the total number of players and bots must be strictly greater than zero");

    int counter = -1;
    for(int i = 0; i < num_players; ++i) {
        ++counter;
        snakes_.emplace_back();
        SpawnNewSnake(snakes_.back(), &counter);
        hcontrol_.emplace_back(&(snakes_.back()));
    }

    for(int i = 0; i < num_bots; ++i) {
        ++counter;
        snakes_.emplace_back();
        SpawnNewSnake(snakes_.back(), &counter);
    }

    FillSnakesColor();
}

void Model::Update()
{
    ClearOldUpdates();
    MoveSnakes();
    SnakesUpdate();
    RemoveDeadSnakes();
    if(!snakes_.empty()) GenerateRabbits();
}

void Model::ClearOldUpdates() { updates_.clear(); }
void Model::MoveSnakes()
{
    for(auto&& snake_it : snakes_) {
        const Coord old_head = snake_it.body_.front();
        snake_it.prev_tail_ = snake_it.body_.back();
        snake_it.Move();

        updates_.push_back({old_head, snake_it.color_,
                            UpdKind::SNAKE_BODY, Direction::UNKNOWN});
        updates_.push_back({snake_it.prev_tail_, ObjColor::WITHOUT_COLOR,
                            UpdKind::EMPTY, Direction::UNKNOWN});
    }
}

void Model::RemoveDeadSnakes()
{
    auto it = snakes_.cbegin(), end = snakes_.cend();
    for( ; it != end; ) {
        if(!it->is_live_) {
            auto body = it->body_.cbegin(), tail = it->body_.cend();
            Updates updates{};
            if(SnakesOverlapped(*body, updates)) {
                updates_.push_back(updates);
                ++body;
            }
            for(; body != tail; ++body) {
                    updates_.push_back({*body, ObjColor::WITHOUT_COLOR,
                                        UpdKind::EMPTY, Direction::UNKNOWN});
            }
            it = snakes_.erase(it);
        }
        else ++it;
    }

    if(snakes_.empty()) game_over_ = true;
}

void Model::GenerateRabbits()
{
    if(rabbits_.size() < rabb_per_snake_ * snakes_.size()) {
        std::pair<Coord, Coord> corners;
        corners.first.x = 1;
        corners.first.y = 1;

        corners.second.x = win_size_.x - 2;
        corners.second.y = win_size_.y - 2;
        
        Coord rabbit_coord = GetRandomCoord(corners.first, corners.second, gen_);
        if(!SnakesOverlapped(rabbit_coord) && !RabbitsOverlapped(rabbit_coord)) {
            rabbits_.emplace_back(rabbit_coord);
            updates_.push_back({rabbit_coord, ObjColor::WITHOUT_COLOR,
                                UpdKind::RABBIT, Direction::UNKNOWN});
        }
    }
}

void Model::SpawnNewSnake(Snake& snake, int* counter)
{
    auto corners_coord = GetSector(*counter);
    Coord head = GetRandomCoord(corners_coord.first, corners_coord.second, gen_);
    Direction dir = GetRandomDirection(gen_);
    snake.dir_ = dir;
    
    switch(dir) {
        case Direction::UP:
            InsertSnake(snake,
                        {head.x, head.y},
                        {head.x, head.y + 1},
                        {head.x, head.y + 2});
            break;
        
        case Direction::DOWN:
            InsertSnake(snake,
                        {head.x, head.y},
                        {head.x, head.y - 1},
                        {head.x, head.y - 2});
            break;
        
        case Direction::RIGHT:
            InsertSnake(snake,
                        {head.x,     head.y},
                        {head.x - 1, head.y},
                        {head.x - 2, head.y});
            break;

        case Direction::LEFT:
            InsertSnake(snake,
                        {head.x,     head.y},
                        {head.x + 1, head.y},
                        {head.x + 2, head.y});
            break;
        
        default: std::cerr << "Error: uknown directions" << std::endl;
    }
}

// counter начинается от 0 (у первой змейки  counter = 0)
std::pair<Coord, Coord> Model::GetSector(int counter)
{
    const int num_snakes = num_bots_ + num_players_;

    if (counter < 0 || counter >= num_snakes)
        throw std::runtime_error("Error: snake index is out of range");

    const int inner_left   = 1;
    const int inner_top    = 1;
    const int inner_width  = win_size_.x - 2;
    const int inner_height = win_size_.y - 2;
    const int snake_margin = 2;

    auto split_range = [](int begin, int length, int index, int parts) {
        const int part_begin = begin + (length * index) / parts;
        const int part_end   = begin + (length * (index + 1)) / parts - 1;
        return std::pair<int, int>{part_begin, part_end};
    };

    int cols = 0;
    int rows = 0;

    if (num_snakes <= 3) {
        cols = num_snakes;
        rows = 1;
    }
    else if (num_snakes == 4) {
        cols = 2;
        rows = 2;
    }
    else if (num_snakes == 5 || num_snakes == 6) {
        cols = 3;
        rows = 2;
    }
    else { 
        // TODO добавить вместо числа константную переменную
        throw std::runtime_error("Error: total number of snakes "
                                 "must be less than or equal to 6\n");
    }

    const int col = counter % cols;
    const int row = counter / cols;

    const auto [sector_left, sector_right] =
        split_range(inner_left, inner_width, col, cols);

    const auto [sector_top, sector_bottom] =
        split_range(inner_top, inner_height, row, rows);

    Coord left_top {
        sector_left + snake_margin,
        sector_top + snake_margin
    };

    Coord right_bottom {
        sector_right - snake_margin,
        sector_bottom - snake_margin
    };

    if (left_top.x > right_bottom.x || left_top.y > right_bottom.y)
        throw std::runtime_error("Error: spawn sector is too small for a snake");

    return {left_top, right_bottom};
}

void Model::InsertSnake(Snake& snake,
                        Coord head, Coord second_part, Coord third_part)
{
    snake.body_.emplace_back(head.x,        head.y);
    snake.body_.emplace_back(second_part.x, second_part.y);
    snake.body_.emplace_back(third_part.x,  third_part.y);
}

bool Model::SnakesOverlapped(Coord coord,
    std::list<Snake>::iterator& current_snake) const
{
    for(auto snake = snakes_.cbegin(), last_snake = snakes_.cend();
        snake != last_snake; ++snake) { 

        auto body_part = snake->body_.cbegin();
        if(snake == current_snake)
            body_part += 1;
            
        for(auto end_part = snake->body_.cend(); body_part !=  end_part; ++body_part) {
            if(*body_part == coord)
                return true;
            
            else continue;
        }
    }
    return false;
}

bool Model::SnakesOverlapped(Coord coord, Updates& updates) const
{
    for(auto&& snake : snakes_) {
        if(!snake.is_live_) continue;
        for(auto&& body_part : snake.body_)
            if(body_part == coord) {
                updates = {body_part, snake.color_, UpdKind::SNAKE_BODY, Direction::UNKNOWN};
                return true;
            }
            else continue;
    }
        
    return false;
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

bool Model::RabbitsOverlapped(Coord coord) const
{
    for(auto rabbit_iter = rabbits_.cbegin(), cend = rabbits_.cend(); rabbit_iter != cend; ++rabbit_iter)
        if(rabbit_iter->body_ == coord)
            return true;
        else continue;

    return false;
}

void Model::Crashes(std::list<Snake>::iterator& it, Coord head_coord)
{
    if(SnakesOverlapped(head_coord, it)) {
        ZeroizeHContrSnake(it);
        it->is_live_ = false;
    }
    return;
}

void Model::BoundariesTeleportation(Snake& snake, Coord coord)
{
    // Top border
    if(coord.y == 0) {
        snake.body_.pop_front();
        snake.body_.emplace_front(coord.x, win_size_.y - 2);
    }

    // Bottom border
    if(coord.y == win_size_.y - 1) {
        snake.body_.pop_front();
        snake.body_.emplace_front(coord.x, 1);
    }

    // Left border
    if(coord.x == 0) {
        snake.body_.pop_front();
        snake.body_.emplace_front(win_size_.x - 2, coord.y);
    }

    // Rignt border
    if(coord.x == win_size_.x - 1) {
        snake.body_.pop_front();
        snake.body_.emplace_front(1, coord.y);
    }
}

void Model::SnakesUpdate()
{
    for(auto snake_it = snakes_.begin(), end = snakes_.end(); snake_it != end;
        ++snake_it) {
        // Direction dir = snake_it->move_algorythm_();
        // snake_it->ChangeDir(dir); // TODO добавить алгоритмы
        BoundariesTeleportation(*snake_it, snake_it->body_.front());
        Coord head_coord = snake_it->body_.front();

        updates_.push_back({head_coord, snake_it->color_,
                            UpdKind::SNAKE_HEAD, snake_it->dir_});

        Crashes(snake_it, head_coord);
        if(snake_it->is_live_) {
            EatingRabbits(*snake_it, head_coord);
        }
    }
}

void Model::EatingRabbits(Snake& snake, Coord new_head_coord)
{
    std::vector<Rabbit>::const_iterator iter;
    if(RabbitsOverlapped(new_head_coord, iter)) {

        rabbits_.erase(iter);
    
        // возвращаем клетку хвоста, которую убрали на ходе выше
        snake.body_.emplace_back(snake.prev_tail_);
        updates_.push_back({snake.prev_tail_, snake.color_,
                            UpdKind::SNAKE_BODY, Direction::UNKNOWN});
    }
}

void Model::ZeroizeHContrSnake(std::list<Snake>::iterator it)
{
    for(auto&& elem : hcontrol_)
        if(elem == &(*it))
            elem = nullptr;
}

void Model::FillSnakesColor()
{
    std::uint8_t i = 0;
    for(auto&& snake : snakes_) {
        snake.color_ = static_cast<ObjColor>(i % static_cast<uint8_t>(ObjColor::WITHOUT_COLOR));
        ++i;
    }
}

bool Model::IsGameOver() { return game_over_; }

} // namespace snake_game

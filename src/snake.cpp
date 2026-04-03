#include "snake.hpp"
#include "coord.hpp"

namespace snake_game {

Snake::Snake(Direction dir) : dir_(dir) {}

void Snake::ChangeDir(Direction dir)
{
    if(body_.front() + dir == body_[1])
        return;
    else dir_ = dir;
}

void Snake::Move()
{
    Coord old_head = body_.front();
    body_.emplace_front(old_head += dir_);
    body_.pop_back();
}

} // namespace snake_game

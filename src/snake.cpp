#include "snake.hpp"
#include "coord.hpp"

namespace snake_game {

Snake::Snake(Direction dir) : dir_(dir) {}

void Snake::ChangeDir(Direction dir)
{
    if(IsRotate180Degrees(dir)) return;
    dir_ = dir;
}

void Snake::Move()
{
    Coord old_head = body_.front();
    body_.emplace_front(old_head += dir_);
    body_.pop_back();
}

bool Snake::IsRotate180Degrees(Direction dir) const
{
    switch(dir) {
        case Direction::LEFT:
            if(dir_ == Direction::RIGHT)
                return true;
            else return false;

        case Direction::RIGHT: 
            if(dir_ == Direction::LEFT)
                return true;
            else return false;

        case Direction::UP:
            if(dir_ == Direction::DOWN)
                return true;
            else return false;

        case Direction::DOWN: 
            if(dir_ == Direction::UP)
                return true;
            else return false;
    }
}


} // namespace snake_game
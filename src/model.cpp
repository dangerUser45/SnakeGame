#include "model.hpp"
#include "coord.hpp"

namespace snake_game {

Model::Model(Coord win_size,
             int num_bots,
             int rabb_per_snake,
             PlayersMode players_mode) :
    win_size_(win_size),
    players_mode_(players_mode),
    rabb_per_snake_(rabb_per_snake)
{
    snakes_.emplace_back();
    SpawnFirstPlayerSnake(snakes_.front());
    hcontrol_.emplace_back(&snakes_.front());

    if(!IsSinglePlayer()) {
        snakes_.emplace_back();
        SpawnSecondPlayerSnake(snakes_.back());
        hcontrol_.emplace_back(&(snakes_.back()));
    }

    for(int i = 0; i < num_bots; ++i) {
        snakes_.emplace_back();
        SpawnNewSnake(snakes_.back());
    }

    FillSnakesColor();
}

void Model::Update()
{
    SnakesUpdate();
    for(auto&& snake : snakes_) snake.Move();
    GenerateRabbits();
}

void Model::GenerateRabbits()
{
    if(rabbits_.size() < rabb_per_snake_ * snakes_.size()) {
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

namespace {}

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

void Model::SpawnNewSnake(Snake& snake)
{
    Coord head = GetRandomCoord({win_size_.x - 2, win_size_.y - 2});
    Direction dir = GetRandomDirection();
    
    // Проверяем в этом направлении есть ли змейки на линии 2-ух клеток
    bool is_located = false;
    for(int i = 0; i < 4; ++i) {
        switch(dir) {
            case Direction::UP:
                InsertSnake(snake, is_located,
                            {head.x, head.y - 1}, {head.x, head.y - 2});
                break;
            
            case Direction::DOWN:
                InsertSnake(snake, is_located,
                            {head.x, head.y + 1}, {head.x, head.y + 2});
                break;
            
            case Direction::RIGHT:
                InsertSnake(snake, is_located,
                            {head.x - 1, head.y}, {head.x - 2, head.y});
                break;

            case Direction::LEFT:
                InsertSnake(snake, is_located,
                            {head.x + 1, head.y}, {head.x + 2, head.y});
                break;
            
            default: std::cerr << "Error: uknown directions" << std::endl;
        }
        if(is_located) break;
        else dir = RotateDir(dir);
    }
    if(!is_located) // TODO добавить обработку ошибки
        std::cerr << "Error: bot can't placement" << std::endl;

    snake.dir_ = dir;
    
    /* DEBUG */ std::cerr << "Im final Spawn function" << "\n" << std::flush;

}

void Model::InsertSnake(Snake& snake, bool& is_located,
                        Coord second_part, Coord third_part)
{
 if(!SnakesOverlapped(second_part) &&
    !SnakesOverlapped(third_part)) {
        snake.body_.emplace_back(second_part.x, second_part.y);
        snake.body_.emplace_back(third_part.x,   third_part.y);
        is_located = true;
    }   
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

bool Model::Crashes(std::list<Snake>::iterator& it, Coord new_head_coord)
{
    if(SnakesOverlapped(new_head_coord)) {
        // TODO добавить какую-нибудь надпись по типу: "О нет, змейка №X умерла!"
        ZeroizeHContrSnake(it);
        it = snakes_.erase(it);

        /* DEBUG */  std::cerr << "Snake is erase \n" << std::flush;
        /* DEBUG */  std::cout << "\033[" << win_size_.y + 2 << ";" << 2 << "H" << "Змейки столкнулись" << std::flush;

        return true;
    }
    return false;
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
    for(auto snake_it = snakes_.begin(), end = snakes_.end(); snake_it != end;) {
        // Direction dir = snake_it->move_algorythm_();
        // snake_it->ChangeDir(dir);

        Coord new_head_coord = snake_it->body_.front() + snake_it->dir_;

        EatingRabbits(*snake_it, new_head_coord);
        BoundariesTeleportation(*snake_it, new_head_coord);
        
        if(!Crashes(snake_it, new_head_coord)) ++snake_it;
    }
}

void Model::EatingRabbits(Snake& snake, Coord new_head_coord)
{
    std::vector<Rabbit>::const_iterator iter;
    if(RabbitsOverlapped(new_head_coord, iter)) {

        rabbits_.erase(iter);
    
        // добавляем элемент в конец змейки, т.к. она съела кролика
        snake.body_.emplace_back();
    }
}

void Model::ZeroizeHContrSnake(std::list<Snake>::iterator it)
{
    for(auto&& elem : hcontrol_)
        if(elem == &(*it))
            elem = nullptr;
}

Direction RotateDir(Direction dir)
{
    switch(dir) {
        case Direction::UP:    return Direction::RIGHT;
        case Direction::RIGHT: return Direction::DOWN;
        case Direction::DOWN:  return Direction::LEFT;
        case Direction::LEFT:  return Direction::UP;

        default: std::cerr << "Error: uknown direction" << std::endl;
            return Direction::UNKNOWN;
    }
}

void Model::FillSnakesColor()
{
    std::uint8_t i = 0;
    for(auto&& snake : snakes_) {
        snake.color_ = static_cast<ObjColor>(i % static_cast<uint8_t>(ObjColor::COUNT));
        ++i;
    }
}

} // namespace snake_game

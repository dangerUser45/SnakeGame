#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <utility>

#include "coord.hpp"
#include "decor.hpp"
#include "limits.hpp"
#include "model.hpp"
#include "snake.hpp"

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
        SpawnNewSnake(snakes_.back(), counter);
        hcontrol_.emplace_back(&(snakes_.back()));
    }

    for(int i = 0; i < num_bots; ++i) {
        ++counter;
        snakes_.emplace_back();
        SpawnNewSnake(snakes_.back(), counter);
    }

    SetSnakesColor();
    SetSnakesBotAlorithms();

    std::uniform_int_distribution<int> num_tics(1, 15);
    power_spawn_capacity_ = num_tics(gen_);
}

void Model::Update()
{
    ClearOldUpdates();
    MoveSnakes();
    SnakesUpdate();
    RemoveDeadSnakes();
    if(!snakes_.empty()) GenerateRabbits();
    // if(IsSpawnPower()) GeneratePowers(); // TODO
}

void Model::ClearOldUpdates() { updates_.clear(); }
void Model::MoveSnakes()
{
    for(auto&& snake_it : snakes_) {
        const Coord old_head = snake_it.body_.front();
        snake_it.prev_tail_ = snake_it.body_.back();
        snake_it.Move();

        updates_.push_back({old_head, snake_it.color_,
                            UpdKind::SNAKE_BODY, Direction::UNDEFINED});
        updates_.push_back({snake_it.prev_tail_, ObjColor::WITHOUT_COLOR,
                            UpdKind::EMPTY, Direction::UNDEFINED});
    }
}

bool Model::IsSpawnPower()
{
    // if(power_spawn_counter_ < power_spawn_capacity_ ||
    //    powers_.size() == 1) {
    //     ++power_spawn_counter_;
    //     return false;
    // }

    // else {
    //     power_spawn_counter_ = 0;
    //     std::uniform_int_distribution<int> num_tics(1, 15);
    //     power_spawn_capacity_ = num_tics(gen_);
    //     return true;
    // }

    // if(powers_.size() == 1) return false;
    // else {
    //     if(power_spawn_counter_ < power_spawn_capacity_)
    //         return false;
    //     else 
    // }
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
                                        UpdKind::EMPTY, Direction::UNDEFINED});
            }
            it = snakes_.erase(it);
        }
        else ++it;
    }

    if(snakes_.empty()) game_over_ = true;
}

void Model::GeneratePowers()
{
    std::pair<Coord, Coord> corners;
    corners.first.x = 1;
    corners.first.y = 1;

    corners.second.x = win_size_.x - 2;
    corners.second.y = win_size_.y - 2;
    
    Coord power_coord = GetRandomCoord(corners.first, corners.second, gen_);
    if(!SnakesOverlapped(power_coord) && !RabbitsOverlapped(power_coord)) {
        powers_.emplace_back(power_coord);
        updates_.push_back({power_coord, ObjColor::PURPLE,
                            UpdKind::POWER, Direction::UNDEFINED});
    }
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
        if(!SnakesOverlapped(rabbit_coord) && !RabbitsOverlapped(rabbit_coord)
        && !PowersOverlapped(rabbit_coord)){
            rabbits_.emplace_back(rabbit_coord);
            updates_.push_back({rabbit_coord, ObjColor::WITHOUT_COLOR,
                                UpdKind::RABBIT, Direction::UNDEFINED});
        }
    }
}

void Model::SpawnNewSnake(Snake& snake, int& counter)
{
    auto corners_coord = GetSector(counter);
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
        std::string err =
            "Error: total number of snakes must be less than or equal to "
            + std::to_string(limits::MAX_NUM_BOTS + limits::MAX_NUM_PLAYERS)
            + "\n";
        throw std::runtime_error(err);
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
                updates = {body_part, snake.color_, UpdKind::SNAKE_BODY, Direction::UNDEFINED};
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

bool Model::PowersOverlapped(Coord coord) const
{
    for(auto power_iter = powers_.cbegin(), cend = powers_.cend(); power_iter != cend; ++power_iter)
        if(power_iter->body_ == coord)
            return true;
        else continue;

    return false;
}

bool Model::PowersOverlapped(Coord coord, std::vector<Power>::const_iterator& iter) const
{
    for(auto power_iter = powers_.cbegin(), cend = powers_.cend(); power_iter != cend; ++power_iter)
        if(power_iter->body_ == coord) {
            iter = power_iter;
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
    for(auto killer = snakes_.begin(), end = snakes_.end(); killer != end; ++killer) {
        auto body_part = killer->body_.cbegin();
        if(killer == it)
            ++body_part;

        for(auto body_end = killer->body_.cend(); body_part != body_end; ++body_part)
            if(*body_part == head_coord) {
                if(killer != it)
                    ++killer->kill_num;

                ZeroizeHContrSnake(it);
                it->is_live_ = false;
                return;
            }
    }
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
        BoundariesTeleportation(*snake_it, snake_it->body_.front());
        Coord head_coord = snake_it->body_.front();

        if(snake_it->bot_ != BotAlgorithm::NOT_BOT) {
            Direction dir = BotAlgorithm(*snake_it);
            snake_it->ChangeDir(dir);
        }

        updates_.push_back({head_coord, snake_it->color_,
                            UpdKind::SNAKE_HEAD, snake_it->dir_});

        Crashes(snake_it, head_coord);
        if(snake_it->is_live_) {
            EatingRabbits(*snake_it, head_coord);
            ActivatingPower(*snake_it, head_coord);
        }
    }
}

void Model::ActivatingPower(Snake& snake, Coord new_head_coord)
{
    std::vector<Power>::const_iterator iter;
    if(PowersOverlapped(new_head_coord, iter)) {
        powers_.erase(iter);
        snake.is_power_activ = true;
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
                            UpdKind::SNAKE_BODY, Direction::UNDEFINED});
    }
}

void Model::ZeroizeHContrSnake(std::list<Snake>::iterator it)
{
    for(auto&& elem : hcontrol_)
        if(elem == &(*it))
            elem = nullptr;
}

void Model::SetSnakesColor()
{
    std::size_t i = 0;
    for(auto&& snake : snakes_) {
        snake.color_ = static_cast<enum ObjColor>(i % static_cast<std::size_t>(ObjColor::WITHOUT_COLOR));
        ++i;
    }
}

void Model::SetSnakesBotAlorithms()
{
    std::size_t counter = 0;
    int cnt = 0;
    for(auto&& snake : snakes_) {
        for(auto&& hcontrol : hcontrol_)
            if(&snake != hcontrol)
                ++cnt;

        if(cnt == num_players_) {
            snake.bot_ = static_cast<enum BotAlgorithm>(counter % static_cast<std::size_t>(BotAlgorithm::NOT_BOT));
            ++counter;
        }
        else snake.bot_ = BotAlgorithm::NOT_BOT;
        cnt = 0;
    }
}

Direction Model::BotAlgorithm(Snake& snake) const
{
    switch(snake.bot_) {
        case BotAlgorithm::DUMB:   return DumbBot(snake);
        case BotAlgorithm::MEDIUM: return MediumBot(snake);
        case BotAlgorithm::SMARTY: return SmartyBot(snake);
        
        default: return Direction::UNDEFINED;
    }
}

namespace {

std::size_t Distance(Coord first, Coord second)
{
    return std::abs(first.x - second.x) + std::abs(first.y - second.y);
}

Rabbit FindNearestRabbit(const Coord& head, const std::vector<Rabbit>& rabbits)
{
    std::size_t min_dist = std::numeric_limits<std::size_t>::max();

    Rabbit nearest_rabbit({0, 0});

    for(auto&& rabbit : rabbits) {
        auto dist = Distance(head, rabbit.body_);
        if(dist < min_dist) {
            min_dist = dist;
            nearest_rabbit = rabbit;
        }
    }

    return nearest_rabbit;
}

Direction GetRabbitDirection(const Coord coord_rabbit,
    const Coord& head, const Direction& default_dir)
{
    int delta_x = coord_rabbit.x - head.x;
    int delta_y = coord_rabbit.y - head.y;

    Direction dir = default_dir;

    if(std::abs(delta_x) > std::abs(delta_y)) {
        if(delta_x > 0) dir = Direction::RIGHT;
        else            dir = Direction::LEFT;
    }
    else {
        if(delta_y > 0) dir = Direction::DOWN;
        else            dir = Direction::UP;
    }

    return dir;
}

Direction RotateDir(const Direction dir)
{
    switch(dir) {
        case Direction::LEFT:  return Direction::UP;
        case Direction::RIGHT: return Direction::DOWN;
        case Direction::UP:    return Direction::RIGHT;
        case Direction::DOWN:  return Direction::LEFT;
        
        default: return Direction::UNDEFINED;
    }
}

using dangermap_t = typename std::vector<std::vector<int>>;
using blockmap_t  = typename std::vector<std::vector<bool>>;
inline constexpr std::array<Direction, 4> kDirections{
    Direction::LEFT,
    Direction::RIGHT,
    Direction::UP,
    Direction::DOWN
};

template <typename T>
std::unique_ptr<std::vector<std::vector<T>>> CreateMap(int size_x, int size_y, T init_value)
{
    return std::make_unique<std::vector
        <std::vector<T>>>(size_y,std::vector<T>(size_x, init_value));
}

struct AStarResult {
    std::vector<Coord> path;
    std::size_t cost = 0;
    bool found = false;
};

bool IsInsidePlayable(Coord coord, int width, int height)
{
    return coord.x >= 1
        && coord.y >= 1
        && coord.x <= width - 2
        && coord.y <= height - 2;
}

Coord WrapPlayableCoord(Coord coord, int width, int height)
{
    if(coord.x <= 0)
        coord.x = width - 2;
    else if(coord.x >= width - 1)
        coord.x = 1;

    if(coord.y <= 0)
        coord.y = height - 2;
    else if(coord.y >= height - 1)
        coord.y = 1;

    return coord;
}

Direction GetNextDirection(Coord head,
                           Coord next_coord,
                           int width,
                           int height,
                           Direction fallback_dir)
{
    for(const Direction dir : kDirections) {
        Coord candidate = WrapPlayableCoord(head + dir, width, height);
        if(candidate == next_coord)
            return dir;
    }

    return fallback_dir;
}

struct AStarNode final {
    Coord coord{};
    int g_score = 0;
    int f_score = 0;
};

struct AStarNodeCompare final {
    bool operator()(const AStarNode& lhs, const AStarNode& rhs) const
    {
        if(lhs.f_score == rhs.f_score)
            return lhs.g_score > rhs.g_score;
        return lhs.f_score > rhs.f_score;
    }
};

AStarResult AStar(Coord start,
                  Coord goal,
                  const dangermap_t& danger_map,
                  const blockmap_t& block_map)
{
    AStarResult result{};

    if(block_map.empty() || block_map.front().empty())
        return result;

    if(danger_map.empty() || danger_map.front().empty())
        return result;

    const int height = static_cast<int>(block_map.size());
    const int width = static_cast<int>(block_map.front().size());

    if(height <= 2 || width <= 2)
        return result;

    if(static_cast<int>(danger_map.size()) < height
    || static_cast<int>(danger_map.front().size()) < width)
        return result;

    start = WrapPlayableCoord(start, width, height);
    goal = WrapPlayableCoord(goal, width, height);

    if(!IsInsidePlayable(start, width, height)
    || !IsInsidePlayable(goal, width, height))
        return result;

    if(start == goal) {
        result.found = true;
        result.cost = 0;
        return result;
    }

    constexpr int kInf = std::numeric_limits<int>::max() / 4;
    auto g_scores = CreateMap<int>(width, height, kInf);
    auto closed = CreateMap<bool>(width, height, false);
    auto parent = std::make_unique<std::vector<std::vector<Coord>>>(
        height, std::vector<Coord>(width, Coord{-1, -1}));

    std::priority_queue<AStarNode,
                        std::vector<AStarNode>,
                        AStarNodeCompare> open_set{};

    (*g_scores)[start.y][start.x] = 0;
    open_set.push({start, 0, static_cast<int>(Distance(start, goal))});

    while(!open_set.empty()) {
        const AStarNode current = open_set.top();
        open_set.pop();

        const Coord current_coord = current.coord;
        if((*closed)[current_coord.y][current_coord.x])
            continue;

        if(current.g_score > (*g_scores)[current_coord.y][current_coord.x])
            continue;

        (*closed)[current_coord.y][current_coord.x] = true;

        if(current_coord == goal) {
            result.found = true;
            result.cost = static_cast<std::size_t>(current.g_score);

            Coord cursor = goal;
            while(!(cursor == start)) {
                result.path.push_back(cursor);
                const Coord prev = (*parent)[cursor.y][cursor.x];
                if(prev.x < 0 || prev.y < 0) {
                    result.found = false;
                    result.path.clear();
                    result.cost = 0;
                    return result;
                }

                cursor = prev;
            }

            std::reverse(result.path.begin(), result.path.end());
            return result;
        }

        for(const Direction dir : kDirections) {
            Coord stepped = current_coord;
            stepped += dir;
            const Coord neighbor = WrapPlayableCoord(stepped, width, height);
            if(!IsInsidePlayable(neighbor, width, height))
                continue;

            if(block_map[neighbor.y][neighbor.x] && !(neighbor == goal))
                continue;

            if((*closed)[neighbor.y][neighbor.x])
                continue;

            const int step_cost = 1 + std::max(0, danger_map[neighbor.y][neighbor.x]);
            const int tentative_g = current.g_score + step_cost;

            if(tentative_g < (*g_scores)[neighbor.y][neighbor.x]) {
                (*g_scores)[neighbor.y][neighbor.x] = tentative_g;
                (*parent)[neighbor.y][neighbor.x] = current_coord;

                const int heuristic = static_cast<int>(Distance(neighbor, goal));
                open_set.push({neighbor, tentative_g, tentative_g + heuristic});
            }
        }
    }

    return result;
}

} // namespace

std::unique_ptr<blockmap_t> Model::BuildBlockMap() const
{
    auto block_map_ptr = CreateMap<bool>(win_size_.x, win_size_.y, false);
    auto& block_map = *block_map_ptr;
    const int height = static_cast<int>(block_map.size());
    const int width = block_map.empty() ? 0 : static_cast<int>(block_map.front().size());

    for(auto&& snake : snakes_)
        for(auto&& bodypart : snake.body_)
            if(bodypart.y >= 0 && bodypart.y < height
            && bodypart.x >= 0 && bodypart.x < width)
                block_map[bodypart.y][bodypart.x] = true;

    return block_map_ptr;
}

std::unique_ptr<dangermap_t> Model::BuildDangerMap() const
{
    auto danger_map_ptr = CreateMap<int>(win_size_.x + 2, win_size_.y + 2, 0);
    auto& danger_map = *danger_map_ptr;
    
    const int danger_score = 20;
    const auto add_danger = [&](int x, int y) {
        if(y < 0 || y >= static_cast<int>(danger_map.size()))
            return;
        if(x < 0 || x >= static_cast<int>(danger_map[0].size()))
            return;

        danger_map[y][x] = danger_score;
    };

    for(auto&& snake : snakes_) {
        const Coord head = snake.body_[0];
        add_danger(head.x - 1, head.y - 1);
        add_danger(head.x + 0, head.y - 1);
        add_danger(head.x + 1, head.y - 1);
        add_danger(head.x + 1, head.y + 0);
        add_danger(head.x + 1, head.y + 1);
        add_danger(head.x + 0, head.y + 1);
        add_danger(head.x - 1, head.y + 1);
        add_danger(head.x - 1, head.y + 0);
    }

    return danger_map_ptr;
}

std::unique_ptr<std::vector<Model::RabbitDistance>>
Model::GetRabbitCandidates(Coord head, std::size_t num_rabbits) const
{ 
    auto candidates = std::make_unique<std::vector<RabbitDistance>>();

    for (const auto& rabbit : rabbits_) {
        std::size_t dist = Distance(head, rabbit.body_);
        candidates->push_back({&rabbit, dist});
    }

    std::sort(candidates->begin(), candidates->end(),
        [](const RabbitDistance& lhs, const RabbitDistance& rhs) {
            return lhs.distance < rhs.distance;
        });

    if (candidates->size() > num_rabbits) {
        candidates->resize(num_rabbits);
    }

    return candidates;
}

Direction Model::DumbBot(Snake& snake) const
{
    const Coord head = snake.body_[0];
    Rabbit rabbit = FindNearestRabbit(head, rabbits_);
    return GetRabbitDirection(rabbit.body_, head, snake.dir_);
}

Direction Model::MediumBot(Snake& snake) const
{
    const Coord head = snake.body_[0];
    const Rabbit nearest_rabbit = FindNearestRabbit(head, rabbits_);
    Direction dir = GetRabbitDirection(nearest_rabbit.body_, head, snake.dir_);

    if(SnakesOverlapped(static_cast<Coord>(head) + dir))
        return RotateDir(dir);

    return dir;
}

Direction Model::SmartyBot(Snake& snake) const
{
    const Coord head = snake.body_[0];
    auto rabbit_cand = GetRabbitCandidates(head, 10);
    auto blockmap  = BuildBlockMap();
    auto dangermap = BuildDangerMap();

    if(rabbit_cand->empty() || blockmap->empty() || blockmap->front().empty())
        return MediumBot(snake);

    if(head.y >= 0
    && head.y < static_cast<int>(blockmap->size())
    && head.x >= 0
    && head.x < static_cast<int>(blockmap->front().size()))
        (*blockmap)[head.y][head.x] = false;

    const int height = static_cast<int>(blockmap->size());
    const int width = static_cast<int>(blockmap->front().size());

    std::size_t best_cost = std::numeric_limits<std::size_t>::max();
    Direction best_direction = Direction::UNDEFINED;

    for(const auto& rabbit : *rabbit_cand) {
        const AStarResult result =
            AStar(head, rabbit.rabbit->body_, *dangermap, *blockmap);

        if(!result.found || result.path.empty())
            continue;

        const Direction candidate_dir = GetNextDirection(
            head,
            result.path.front(),
            width,
            height,
            snake.dir_);

        if(candidate_dir == Direction::UNDEFINED)
            continue;

        if(result.cost < best_cost) {
            best_cost = result.cost;
            best_direction = candidate_dir;
        }
    }

    if(best_direction != Direction::UNDEFINED)
        return best_direction;

    return MediumBot(snake);
}

bool Model::IsGameOver() { return game_over_; }

} // namespace snake_game

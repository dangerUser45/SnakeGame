#pragma once

#include <cstdint>
#include <vector>

#include "snake.hpp"
#include "rabbit.hpp"


namespace snake_game {

class Model {
public:
    Model(uint64_t width, uint64_t heigth,
          uint64_t num_snake, uint64_t num_rabbits) :
          width_(width), heigth_(heigth) 
    {
        snakes_.reserve(num_snake);
        rabits_.reserve(num_rabbits);
    }

    ~Model() = default;

    void Update()
    {

    }

private:
    uint64_t width_, heigth_;
    std::vector<Snake> snakes_;
    std::vector<Rabbit> rabits_;
};

} // namespace snake_game

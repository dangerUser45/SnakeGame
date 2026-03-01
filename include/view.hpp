#pragma once

#include <vector>

#include "snake.hpp"
#include "rabbit.hpp"

namespace snake_game {

enum class Events {
    KEY_PRESSED_LEFT,
    KEY_PRESSED_RIGHT,
    KEY_PRESSED_UP,
    KEY_PRESSED_DOWN,

    KEY_PRESSED_PAUSE,
    KEY_PRESSED_EXIT
};

class View {
public:

private:
    std::vector<Events> events_;
};

class TextView final : public View {
public:

    void ProccessEvents()
    {
        
    }

    void DrawSnake([[maybe_unused]] Snake& snake)
    {

    }
    void DrawRabbit([[maybe_unused]] Rabbit& rabbit)
    {

    }

    void Render()
    {
        
    }

private:

};

class GraphicalView final : public View {
public:
    void ProccessEvents()
    {
        
    }

    void Render()
    {
        
    }


private:

};

}

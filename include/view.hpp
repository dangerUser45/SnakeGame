#pragma once

#include <termios.h>
#include <memory>
#include <optional>
#include <fcntl.h>
#include <queue>

#include "snake.hpp"
#include "rabbit.hpp"
#include "coord.hpp"
#include "model.hpp"
#include "event.hpp"

#define ESC "\033]"

namespace snake_game {

class View {
public:
    virtual std::optional<Event> PollEvents() = 0;
    virtual void Render(Model&) = 0;
    virtual ~View() = default;
};

class TextView final : public View {
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;    

public:
    std::optional<Event> PollEvents() override;
    void Render(Model& model) override;

    TextView();
    ~TextView();
};

struct TextView::Impl final {
private:
    static const int32_t DEFAULT_WIDTH  = 145;
    static const int32_t DEFAULT_HEIGTH = 34;

    bool is_terminal_configured_{false};
    termios old_term_{};
    std::queue<Event> events_;
    

public:
    void GotoXY(Coord coord) const;
    void GotoXY(int32_t x, int32_t y) const;
    void ClearScreen();
    void DrawSnake(const Snake& snake) const;
    const std::string_view DrawSnakeHead(Direction dir) const;
    void DrawRabbit(const Rabbit& rabbit) const;
    void DrawBackground(const Coord win_size) const;
    void SetupTerminal();
    void RestoreTerminal();
    void HideCursor() const;
    void ShowCursor() const;
    void UpdateEventsBuffer();
    std::optional<Event> PopNextEvent();
};

class GraphicalView final : public View {
public:
    // TODO: сделать ф-цию для graphview
    std::optional<Event> PollEvents() override
    {
        return std::nullopt;
    }

    void Render( Model& model) override
    {
        //TODO убрать заглушку
        (void) model;
    }

private:

};

} // namespace snake_game

#pragma once

#include <termios.h>
#include <queue>

#include "coord.hpp"
#include "model.hpp"
#include "event.hpp"
#include "view.hpp"

namespace snake_game {

class TerminalView final : public View {
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

public:
    std::optional<Event> PollEvents() override;
    void Render(Model& model) override;

    TerminalView();
    ~TerminalView();
};

struct TerminalView::Impl final {
private:
    static const int DEFAULT_WIDTH  = 145;
    static const int DEFAULT_HEIGTH = 34;

    termios old_term_{};
    std::queue<Event> events_{};
    
public:
    bool is_init_drawing_ = true;

    void GotoXY(Coord coord) const;
    void GotoXY(int x, int y) const;
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
    std::string_view GetTerminalColor(ObjColor color) const;
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

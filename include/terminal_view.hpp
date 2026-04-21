#pragma once

#include <queue>
#include <termios.h>

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
    [[nodiscard]] std::optional<Event> PollEvents() override;
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
    bool is_init_rendering_ = true;

    void GotoXY(Coord coord) const;
    void GotoXY(int x, int y) const;
    void GotoXYInit(int x, int y) const;
    void ClearScreen();
    void DrawSnake(const Snake& snake) const;
    [[nodiscard]] const std::string_view DrawSnakeHead(Direction dir) const;
    void DrawRabbit(const Rabbit& rabbit) const;
    void DrawBackground(const Coord win_size) const;
    void SetupTerminal();
    void RestoreTerminal();
    void HideCursor() const;
    void ShowCursor() const;
    void UpdateEventsBuffer();
    void FullRender(Model& model);
    void UpdatesRender(Model&  model);
    [[nodiscard]] const std::string_view DrawUpdate(Model::Updates& update);
    void DrawBanner() const;


    std::string_view GetTerminalColor(ObjColor color) const;
    std::optional<Event> PopNextEvent();
};

} // namespace snake_game

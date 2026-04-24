#pragma once

#include <queue>
#include <string>
#include <termios.h>
#include <vector>

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
    [[nodiscard]] bool CanRenderGameplay(const Model& model) const override;
    void Render(Model& model) override;
    void RenderBotChampionship(const BotChampionshipStats& stats) override;

    TerminalView();
    ~TerminalView();
};

struct TerminalView::Impl final {
private:
    struct SnakeStatsEntry final {
        ObjColor color_ = ObjColor::WITHOUT_COLOR;
        std::string controller_label_{};
        Direction dir_ = Direction::RIGHT;
        int score_ = 0;
        int kills_ = 0;
        bool is_live_ = true;
    };

    static const int DEFAULT_WIDTH  = 145;
    static const int DEFAULT_HEIGTH = 34;

    termios old_term_{};
    std::queue<Event> events_{};
    std::vector<SnakeStatsEntry> snake_stats_{};
    Coord terminal_size_{};
    Coord field_origin_{1, 1};
    Coord banner_origin_{1, 1};
    Coord stats_origin_{1, 1};
    
public:
    bool is_init_rendering_ = true;

    void GotoXY(Coord coord) const;
    void GotoXY(int x, int y) const;
    void GotoXYInit(int x, int y) const;
    void ClearScreen() const;
    void DrawSnake(const Snake& snake) const;
    [[nodiscard]] const std::string_view DrawSnakeHead(Direction dir) const;
    void DrawRabbit(const Rabbit& rabbit) const;
    void DrawBackground(const Coord win_size) const;
    void SetupTerminal();
    void RestoreTerminal();
    void HideCursor() const;
    void ShowCursor() const;
    void UpdateEventsBuffer();
    [[nodiscard]] Coord GetTerminalSize() const;
    bool UpdateTerminalSize();
    void SyncSnakeStats(const Model& model);
    void UpdateLayout(const Model& model);
    [[nodiscard]] Coord GetStatsWindowSize(const Model& model) const;
    [[nodiscard]] Coord GetContentSize(const Model& model) const;
    [[nodiscard]] Coord GetRequiredTerminalSize(const Model& model) const;
    [[nodiscard]] Coord GetMaxSupportedTerminalSize() const;
    [[nodiscard]] Coord GetBotChampionshipSize(const BotChampionshipStats& stats) const;
    void DrawFrame(Coord origin, Coord size) const;
    void DrawStatsWindow(const Model& model) const;
    void DrawBotChampionship(const BotChampionshipStats& stats);
    void DrawViewportWarning(const Model& model) const;
    void FullRender(Model& model);
    void UpdatesRender(Model&  model);
    [[nodiscard]] const std::string_view DrawUpdate(Model::Updates& update);
    void DrawBanner() const;


    std::string_view GetTerminalColor(ObjColor color) const;
    std::optional<Event> PopNextEvent();
};

} // namespace snake_game

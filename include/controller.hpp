#pragma once

#include <unistd.h>

#include "bot_championship.hpp"
#include "model.hpp"
#include "view.hpp"
#include "options.hpp"

namespace snake_game {

class Controller final {
private:
    Model model_;
    std::unique_ptr<View> view_;
    Coord win_size_{};
    int tic_time_ = Model::UNDEFINED_NUM;
    int rabb_per_snake_ = Model::UNDEFINED_NUM;
    int championship_rounds_ = 0;
    bool should_exit_ = false;
    bool is_game_paused_ = false;
    bool is_viewport_blocked_ = false;

    [[nodiscard]] Model BuildChampionshipModel() const;
    void ShowBotChampionshipResults(const BotChampionshipStats& stats);

public:
    Controller(Options& opt);

    void Run();
    void RunBotChampionship(int rounds);
    void ProcessEvents(const Event event);
};

} // namespace snake_game

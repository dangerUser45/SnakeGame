#pragma once

#include <unistd.h>

#include "model.hpp"
#include "view.hpp"
#include "options.hpp"

namespace snake_game {

class Controller final {
private:
    Model model_;
    std::unique_ptr<View> view_;
    bool should_exit_ = false;
    bool is_game_paused_ = false;

public:
    Controller(Options& opt);

    void Run();
    void ProcessEvents(const Event event);
};

} // namespace snake_game

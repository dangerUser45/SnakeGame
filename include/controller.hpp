#pragma once

#include <unistd.h>

#include "model.hpp"
#include "view.hpp"

namespace snake_game {

class Controller final {
private:
    Model& model_;
    View& view_;
    bool game_over_ = false;
    bool is_game_paused = false;

public:
    Controller(Model& model,
               View& view
              );

    void Run();
    void ProcessEvents(const Event event);
};

} // namespace snake_game

#pragma once

#include "model.hpp"

namespace snake_game {

inline const int DEFAULT_NUM_RABBITS = 2;
inline const int DEFAULT_NUM_SNAKES  = 2;
inline const uint64_t DEFAULT_WIDTH  = 256;
inline const uint64_t DEFAULT_HEIGTH = 256;

template <class ViewT>
class Controller final {
public:
    
    // main loop
    void Run()
    {
        while(!game_over_) {
            view_.ProccessEvents();
            model_.Update();
            view_.Render();
            // sleep(model.get_tic_time())
        }
    }

private:

    Model model_{
        DEFAULT_WIDTH,
        DEFAULT_HEIGTH,
        DEFAULT_NUM_SNAKES,
        DEFAULT_NUM_RABBITS};
    
    ViewT view_;

    bool game_over_ = false;
};

} // namespace snake_game

/* NOTE

events обрабатывать не сразу после получения input, а по game тику


если пришёл ивент по типу нажатию нажатия на паузу, то можно сделать так, чтобы
такой ивент обрабатывался мгновенно

*/

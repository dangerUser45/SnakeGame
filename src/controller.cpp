#include <thread>

#include "controller.hpp"
#include "view.hpp"

namespace snake_game {

Controller::Controller(Model& model, View& view)
  : model_(model)
  , view_(view)
  {}

void Controller::Run()
{
    while (!game_over_) {
        while (std::optional<Event> event = view_.PollEvents())
            ProcessEvents(*event);
        
        if(!is_game_paused)
            model_.Update();
        
        view_.Render(model_);
        std::this_thread::sleep_for(model_.tic_time_);
    }
}

void Controller::ProcessEvents(const Event event)
{
    switch(event) {
        // First Player
        case Event::KEY_PRESSED_P1_LEFT:
            model_.snakes_[0].ChangeDir(Direction::LEFT);
            break;

        case Event::KEY_PRESSED_P1_RIGHT:
            model_.snakes_[0].ChangeDir(Direction::RIGHT);
            break;

        case Event::KEY_PRESSED_P1_UP:
            model_.snakes_[0].ChangeDir(Direction::UP);
            break;
        case Event::KEY_PRESSED_P1_DOWN:
            model_.snakes_[0].ChangeDir(Direction::DOWN);
            break;

        // Second Player
        case Event::KEY_PRESSED_P2_LEFT:
            if(model_.is_single_player()) break;
            model_.snakes_[1].ChangeDir(Direction::LEFT);
            break;

        case Event::KEY_PRESSED_P2_RIGHT:
            if(model_.is_single_player()) break;
            model_.snakes_[1].ChangeDir(Direction::RIGHT);
            break;

        case Event::KEY_PRESSED_P2_UP:
            if(model_.is_single_player()) break;
            model_.snakes_[1].ChangeDir(Direction::UP);
            break;

        case Event::KEY_PRESSED_P2_DOWN:
            if(model_.is_single_player()) break;
            model_.snakes_[1].ChangeDir(Direction::DOWN);
            break;

        // Menu actions
        case Event::KEY_PRESSED_PAUSE:
            is_game_paused = !is_game_paused;
            break;

        case Event::KEY_PRESSED_EXIT:
            game_over_ = true;
            break;
        case Event::RESIZE_WINDOW:
            break; // TODO добавить обработку

        default: break;
    }
}
} // namespace snake_game

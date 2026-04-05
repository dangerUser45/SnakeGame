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
    while (!should_exit_) {
        while (std::optional<Event> event = view_.PollEvents())
            ProcessEvents(*event);
        
        if(!is_game_paused_) {
            model_.Update();
            if(model_.IsGameOver())
                should_exit_ = true;
        }
        
        view_.Render(model_);
        std::this_thread::sleep_for(model_.tic_time_);
    }
}

void Controller::ProcessEvents(const Event event)
{
    if(!is_game_paused_) {
        if(model_.num_players_ >= 1)
            if(model_.hcontrol_[0])
                switch(event) {
                    // First Player
                    case Event::KEY_PRESSED_P1_LEFT:
                        model_.hcontrol_[0]->ChangeDir(Direction::LEFT);
                        break;

                    case Event::KEY_PRESSED_P1_RIGHT:
                        model_.hcontrol_[0]->ChangeDir(Direction::RIGHT);
                        break;

                    case Event::KEY_PRESSED_P1_UP:
                        model_.hcontrol_[0]->ChangeDir(Direction::UP);
                        break;

                    case Event::KEY_PRESSED_P1_DOWN:
                        model_.hcontrol_[0]->ChangeDir(Direction::DOWN);
                        break;

                    default: break;
                }
        
        if(model_.num_players_ == 2)
            if(model_.hcontrol_[1])
                switch(event) {
                    // Second Player
                    case Event::KEY_PRESSED_P2_LEFT:
                        model_.hcontrol_[1]->ChangeDir(Direction::LEFT);
                        break;

                    case Event::KEY_PRESSED_P2_RIGHT:
                        model_.hcontrol_[1]->ChangeDir(Direction::RIGHT);
                        break;

                    case Event::KEY_PRESSED_P2_UP:
                        model_.hcontrol_[1]->ChangeDir(Direction::UP);
                        break;

                    case Event::KEY_PRESSED_P2_DOWN:
                        model_.hcontrol_[1]->ChangeDir(Direction::DOWN);
                        break;

                    default: break;
                }
    }

    switch(event) {
        // Menu actions
        case Event::KEY_PRESSED_PAUSE:
            is_game_paused_ = !is_game_paused_;
            break;

        case Event::KEY_PRESSED_EXIT:
            should_exit_ = true;
            break;
        case Event::RESIZE_WINDOW:
            break; // TODO добавить обработку

        default: break;
    }
}
} // namespace snake_game

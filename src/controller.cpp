#include <memory>
#include <thread>

#include "controller.hpp"
#include "model.hpp"
#include "options.hpp"
#include "view.hpp"
#include "terminal_view.hpp"
#include "graphical_view.hpp"

namespace snake_game {

namespace {
Model MakeModel(Options& opt)
{
    return Model::Builder()
    .SetWinSize(opt.win_size())
    .SetViewMode(opt.view_mode())
    .SetNumPlayers(opt.num_players())
    .SetNumBots(opt.num_bots())
    .SetRabbPerSnake(opt.rabb_per_snake())
    .SetTicTime(opt.tic_time())
    .Build();
}

std::unique_ptr<View> MakeView(ViewMode view_mode)
{
    if(view_mode == ViewMode::GRAPHICAL_VIEW)
        return std::make_unique<GraphicalView>();

    else if (view_mode == ViewMode::TERMINAL_VIEW)
        return std::make_unique<TerminalView>();

    else return nullptr;
}

}

Controller::Controller(Options& opt)
try : model_(MakeModel(opt)),
      view_(MakeView(model_.view_mode_))
{}

catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
}

void Controller::Run()
{
    while (!should_exit_) {
        while (std::optional<Event> event = view_->PollEvents())
            ProcessEvents(*event);

        is_viewport_blocked_ = !view_->CanRenderGameplay(model_);
        
        if(!is_game_paused_ && !is_viewport_blocked_) {
            model_.Update();
            if(model_.IsGameOver())
                should_exit_ = true;
        }
        
        view_->Render(model_);
        std::this_thread::sleep_for(model_.tic_time_);
    }
}

void Controller::ProcessEvents(const Event event)
{
    switch(event) {
        // Menu actions
        case Event::KEY_PRESSED_PAUSE:
            is_game_paused_ = !is_game_paused_;
            break;

        case Event::KEY_PRESSED_EXIT:
            should_exit_ = true;
            break;
        case Event::RESIZE_WINDOW:
            is_viewport_blocked_ = !view_->CanRenderGameplay(model_);
            break;

        default: break;
    }

    if(is_game_paused_ || is_viewport_blocked_)
        return;

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
} // namespace snake_game

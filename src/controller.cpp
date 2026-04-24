#include <algorithm>
#include <array>
#include <chrono>
#include <stdexcept>
#include <thread>

#include "controller.hpp"
#include "graphical_view.hpp"
#include "model.hpp"
#include "options.hpp"
#include "terminal_view.hpp"
#include "view.hpp"

namespace snake_game {

namespace {

inline constexpr int ChampionshipNumPlayers = 0;
inline constexpr int ChampionshipNumBots = 3;
inline constexpr int ChampionshipMaxTicks = 500'000;

struct RoundBotState final {
    ObjColor color_ = ObjColor::WITHOUT_COLOR;
    int kills_ = 0;
    bool is_alive_ = true;
    bool was_seen_ = false;
};

std::size_t ToBotIndex(BotAlgorithm algorithm)
{
    switch(algorithm) {
        case BotAlgorithm::DUMB:   return 0;
        case BotAlgorithm::MEDIUM: return 1;
        case BotAlgorithm::SMARTY: return 2;
        default:
            throw std::runtime_error("Error: unsupported bot algorithm in championship");
    }
}

BotChampionshipStats MakeInitialChampionshipStats(int rounds)
{
    BotChampionshipStats stats{};
    stats.rounds_ = rounds;
    stats.max_ticks_per_round_ = ChampionshipMaxTicks;
    stats.entries_ = {
        {BotAlgorithm::DUMB, ObjColor::WITHOUT_COLOR, "Dumb"},
        {BotAlgorithm::MEDIUM, ObjColor::WITHOUT_COLOR, "Medium"},
        {BotAlgorithm::SMARTY, ObjColor::WITHOUT_COLOR, "Smarty"}
    };
    return stats;
}

void SyncRoundBotStates(const Model& model, std::array<RoundBotState, ChampionshipNumBots>& states)
{
    std::array<bool, ChampionshipNumBots> seen{};

    for(const auto& snake : model.snakes_) {
        if(snake.bot_ == BotAlgorithm::NOT_BOT)
            continue;

        const std::size_t index = ToBotIndex(snake.bot_);
        auto& state = states[index];
        state.color_ = snake.color_;
        state.kills_ = snake.kill_num;
        state.is_alive_ = snake.is_live_;
        state.was_seen_ = true;
        seen[index] = true;
    }

    for(std::size_t i = 0; i < states.size(); ++i)
        if(states[i].was_seen_ && !seen[i])
            states[i].is_alive_ = false;
}

void AccumulateRoundStats(BotChampionshipStats& stats,
                          const std::array<RoundBotState, ChampionshipNumBots>& states)
{
    for(std::size_t i = 0; i < stats.entries_.size(); ++i) {
        auto& entry = stats.entries_[i];
        const auto& state = states[i];
        if(state.color_ != ObjColor::WITHOUT_COLOR)
            entry.color_ = state.color_;

        entry.total_kills_ += state.kills_;
        if(state.is_alive_)
            ++entry.survived_rounds_;
    }
}

void SortChampionshipEntries(BotChampionshipStats& stats)
{
    std::sort(stats.entries_.begin(), stats.entries_.end(),
              [&](const BotChampionshipEntry& lhs, const BotChampionshipEntry& rhs) {
                  if(lhs.wins_ != rhs.wins_)
                      return lhs.wins_ > rhs.wins_;
                  if(lhs.total_kills_ != rhs.total_kills_)
                      return lhs.total_kills_ > rhs.total_kills_;
                  return ToBotIndex(lhs.algorithm_) < ToBotIndex(rhs.algorithm_);
              });
}

Model MakeGameplayModel(Options& opt)
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

Model MakeChampionshipBaseModel(Options& opt)
{
    return Model::Builder()
        .SetWinSize(opt.win_size())
        .SetViewMode(opt.view_mode())
        .SetNumPlayers(ChampionshipNumPlayers)
        .SetNumBots(ChampionshipNumBots)
        .SetRabbPerSnake(opt.rabb_per_snake())
        .SetTicTime(opt.tic_time())
        .Build();
}

Model MakeModel(Options& opt)
{
    if(opt.bot_championship_rounds() > 0)
        return MakeChampionshipBaseModel(opt);

    return MakeGameplayModel(opt);
}

std::unique_ptr<View> MakeView(ViewMode view_mode)
{
    if(view_mode == ViewMode::GRAPHICAL_VIEW)
        return std::make_unique<GraphicalView>();

    if(view_mode == ViewMode::TERMINAL_VIEW)
        return std::make_unique<TerminalView>();

    return nullptr;
}

} // namespace

Controller::Controller(Options& opt)
try : model_(MakeModel(opt)),
      view_(MakeView(model_.view_mode_)),
      win_size_(model_.win_size_),
      tic_time_(static_cast<int>(model_.tic_time_.count())),
      rabb_per_snake_(model_.rabb_per_snake_),
      championship_rounds_(opt.bot_championship_rounds())
{}

catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    throw;
}

void Controller::Run()
{
    if(championship_rounds_ > 0) {
        RunBotChampionship(championship_rounds_);
        return;
    }

    while(!should_exit_) {
        while(std::optional<Event> event = view_->PollEvents())
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

Model Controller::BuildChampionshipModel() const
{
    return Model::Builder()
        .SetWinSize(win_size_)
        .SetViewMode(model_.view_mode_)
        .SetNumPlayers(ChampionshipNumPlayers)
        .SetNumBots(ChampionshipNumBots)
        .SetRabbPerSnake(rabb_per_snake_)
        .SetTicTime(tic_time_)
        .Build();
}

void Controller::ShowBotChampionshipResults(const BotChampionshipStats& stats)
{
    should_exit_ = false;
    is_game_paused_ = false;
    is_viewport_blocked_ = false;
    bool should_redraw = true;

    while(!should_exit_) {
        while(const std::optional<Event> event = view_->PollEvents()) {
            if(*event == Event::KEY_PRESSED_EXIT) {
                should_exit_ = true;
                break;
            }

            if(*event == Event::RESIZE_WINDOW)
                should_redraw = true;
        }

        if(should_exit_)
            break;

        if(should_redraw) {
            view_->RenderBotChampionship(stats);
            should_redraw = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void Controller::RunBotChampionship(int rounds)
{
    BotChampionshipStats stats = MakeInitialChampionshipStats(rounds);

    for(int round = 0; round < rounds; ++round) {
        Model round_model = BuildChampionshipModel();
        std::array<RoundBotState, ChampionshipNumBots> round_states{};
        SyncRoundBotStates(round_model, round_states);

        int ticks = 0;
        while(!round_model.IsGameOver()
              && static_cast<int>(round_model.snakes_.size()) > 1
              && ticks < ChampionshipMaxTicks) {
            round_model.Update();
            SyncRoundBotStates(round_model, round_states);
            ++ticks;
        }

        if(static_cast<int>(round_model.snakes_.size()) == 1) {
            const BotAlgorithm winner = round_model.snakes_.front().bot_;
            stats.entries_[ToBotIndex(winner)].wins_ += 1;
        }
        else if(round_model.snakes_.empty()) {
            ++stats.draws_;
        }
        else if(ticks >= ChampionshipMaxTicks) {
            ++stats.timeouts_;
        }

        AccumulateRoundStats(stats, round_states);
    }

    SortChampionshipEntries(stats);
    ShowBotChampionshipResults(stats);
}

void Controller::ProcessEvents(const Event event)
{
    switch(event) {
        case Event::KEY_PRESSED_PAUSE:
            is_game_paused_ = !is_game_paused_;
            break;

        case Event::KEY_PRESSED_EXIT:
            should_exit_ = true;
            break;

        case Event::RESIZE_WINDOW:
            is_viewport_blocked_ = !view_->CanRenderGameplay(model_);
            break;

        default:
            break;
    }

    if(is_game_paused_ || is_viewport_blocked_)
        return;

    if(model_.num_players_ >= 1)
        if(model_.hcontrol_[0])
            switch(event) {
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

                default:
                    break;
            }

    if(model_.num_players_ == 2)
        if(model_.hcontrol_[1])
            switch(event) {
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

                default:
                    break;
            }
}

} // namespace snake_game

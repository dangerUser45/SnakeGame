#pragma once

#include <coord.hpp>
#include <getopt.h>
#include <stdlib.h>

#include "model.hpp"
namespace snake_game {

enum OptionCode {
    NUM_PLAYERS,
    NUM_BOTS,
    TIC_TIME,
    RABB_PER_SNAKE,
    WIN_SIZE,
    VIEW_MODE,
    BOT_CHAMPIONSHIP
};

class Options final {
private:
    long int GetNum(long int min, long int max, const std::string& label);

    const struct option long_options[8] {
        {"num_players",    required_argument, 0, OptionCode::NUM_PLAYERS},
        {"num_bots",       required_argument, 0, OptionCode::NUM_BOTS},
        {"tic_time",       required_argument, 0, OptionCode::TIC_TIME},
        {"rabb_per_snake", required_argument, 0, OptionCode::RABB_PER_SNAKE},
        {"win_size",       required_argument, 0, OptionCode::WIN_SIZE},
        {"view_mode",      required_argument, 0, OptionCode::VIEW_MODE},
        {"bot_championship", required_argument, 0, OptionCode::BOT_CHAMPIONSHIP},
        {0, 0, 0, 0}
    };

    int num_players_    =  Model::UNDEFINED_NUM;
    int num_bots_       =  Model::UNDEFINED_NUM;
    int tic_time_       =  Model::UNDEFINED_NUM; 
    int rabb_per_snake_ =  Model::UNDEFINED_NUM;
    int bot_championship_rounds_ = 0;
    ViewMode view_mode_ =  ViewMode::UNDEFINED_VIEW;
    Coord win_size_     = {Model::UNDEFINED_NUM, Model::UNDEFINED_NUM};

    int opt_ = 0;
    std::string err_message_ = "";
    
public:
    void GetOptions(int argc, char** argv);
    [[nodiscard]] int num_players() const noexcept {return num_players_;}
    [[nodiscard]] int num_bots() const noexcept {return num_bots_;}
    [[nodiscard]] int tic_time() const noexcept {return tic_time_;}
    [[nodiscard]] int rabb_per_snake() const noexcept {return rabb_per_snake_;}
    [[nodiscard]] int bot_championship_rounds() const noexcept {return bot_championship_rounds_;}
    [[nodiscard]] ViewMode view_mode() const noexcept {return view_mode_;}
    [[nodiscard]] Coord win_size() const noexcept {return win_size_;}
};

} //namespace snake_game

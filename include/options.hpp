#pragma once

#include <coord.hpp>
#include <getopt.h>
#include <stdlib.h>

#include "model.hpp"

namespace snake_game {

struct Options final {
    Model::PlayersMode players_mode = Model::PlayersMode::UNDEFINED;
    int num_bots            = Model::UNDEFINED_NUM;
    int rabb_per_snake      = Model::UNDEFINED_NUM;
    Coord win_size                  = Model::UNDEFINED_COORD;
};

void GetOptions(int argc, char** argv, Options& options);

} //namespace snake_game

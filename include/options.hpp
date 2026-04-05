#pragma once

#include <coord.hpp>
#include <getopt.h>
#include <stdlib.h>

#include "model.hpp"

namespace snake_game {

struct Options final {
    int num_players    =   Model::UNDEFINED_NUM;
    int num_bots       =   Model::UNDEFINED_NUM;
    int rabb_per_snake =   Model::UNDEFINED_NUM;
    Coord win_size     = { Model::UNDEFINED_NUM, Model::UNDEFINED_NUM };
};

void GetOptions(int argc, char** argv, Options& options);

} //namespace snake_game

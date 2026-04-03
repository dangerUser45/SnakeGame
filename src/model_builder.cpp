#include "model.hpp"

namespace snake_game {

Model::Builder& Model::Builder::SetWinSize(Coord win_size) 
{
    if(win_size != UNDEFINED_COORD)
        win_size_ = win_size;
    return *this; 
}

Model::Builder& Model::Builder::SetNumBots(int num_bots)
{
    if(num_bots != UNDEFINED_NUM)
        num_bots_ = num_bots; 
    return *this;
}

Model::Builder& Model::Builder::SetRabbPerSnake(int rabb_per_snake)
{
    if(rabb_per_snake != UNDEFINED_NUM)
        rabb_per_snake_ = rabb_per_snake; 
    return *this;
}

Model::Builder& Model::Builder::SetPlayersMode(PlayersMode players_mode)
{
    if(players_mode != PlayersMode::UNDEFINED)
        players_mode_ = players_mode;
    return *this;
}

Model Model::Builder::Build() const
{
    return Model(win_size_, num_bots_, rabb_per_snake_, players_mode_);
}

} // namespace snake_game
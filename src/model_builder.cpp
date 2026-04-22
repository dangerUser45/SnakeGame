#include "model.hpp"

namespace snake_game {

Model::Builder& Model::Builder::SetWinSize(Coord win_size) 
{
    const Coord undefined_coord = {UNDEFINED_NUM, UNDEFINED_NUM};
    if(win_size != undefined_coord)
        win_size_ = win_size;
    return *this;
}

Model::Builder& Model::Builder::SetNumBots(int num_bots)
{
    if(num_bots != UNDEFINED_NUM)
        num_bots_ = num_bots; 
    return *this;
}

Model::Builder& Model::Builder::SetTicTime(int tic_time)
{
    if(tic_time != UNDEFINED_NUM)
        tic_time_ = tic_time; 
    return *this;
}

Model::Builder& Model::Builder::SetRabbPerSnake(int rabb_per_snake)
{
    if(rabb_per_snake != UNDEFINED_NUM)
        rabb_per_snake_ = rabb_per_snake; 
    return *this;
}

Model::Builder& Model::Builder::SetNumPlayers(int num_players)
{
    if(num_players != UNDEFINED_NUM)
        num_players_ = num_players;
    return *this;
}

Model::Builder& Model::Builder::SetViewMode(ViewMode view_mode)
{
    if(view_mode != ViewMode::UNDEFINED_VIEW)
        view_mode_ = view_mode;
    return *this;
}

Model Model::Builder::Build() const
{
    return Model(win_size_, view_mode_, num_players_, num_bots_, tic_time_, rabb_per_snake_);
}

} // namespace snake_game

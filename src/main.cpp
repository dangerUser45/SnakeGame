#include "view.hpp"
#include "controller.hpp"
#include "model.hpp"

int main(int argc, char** argv)
{
    using namespace snake_game;

    /* TODO добавить опции:
        1) количество игроков-змеек
        2) количество ботов-змеек
        3) количество кроликов на одну змейку
        4) размер экрана Width x Heigth
        5) умность ботов
    */
    
    // GetOptions();

    Model model = Model::Builder()
    .SetPlayersMode(Model::PlayersMode::TWO_PLAYER)
    // .SetWinSize({70, 30})
    .Build();

    TextView txt_view{};

    Controller contoller{model, txt_view};
    contoller.Run();
}

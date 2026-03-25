#include <getopt.h>

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

    // TODO добавить проверки в builer (по типу неверн. размер окна, отриц. кол-во змеек и т.д.)
    Model model = Model::Builder()
    // .SetWinSize({70, 30})
    .SetPlayersMode(Model::PlayersMode::TWO_PLAYER)
    .Build();

    TextView txt_view{};

    Controller contoller{model, txt_view};
    contoller.Run();
}

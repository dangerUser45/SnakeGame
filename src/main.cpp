#include "view.hpp"
#include "controller.hpp"
#include "model.hpp"

int main(int argc, char** argv)
{
    using namespace snake_game;

    // GetOptions();

    Model model = Model::Builder()
    .SetPlayersMode(Model::PlayersMode::SINGLE_PLAYER)
    // .SetWinSize({70, 30})
    .Build();

    TextView txt_view{};

    Controller contoller{model, txt_view};
    contoller.Run();
}

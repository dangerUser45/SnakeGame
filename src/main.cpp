#include <exception>
#include <getopt.h>

#include "terminal_view.hpp"
#include "controller.hpp"
#include "options.hpp"
#include "model.hpp"

int main(int argc, char** argv)
{
    using namespace snake_game;
    Options opt{};
    try {
        GetOptions(argc, argv, opt);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    Model model = Model::Builder()
    .SetWinSize(opt.win_size)
    .SetNumBots(opt.num_bots)
    .SetRabbPerSnake(opt.rabb_per_snake)
    .SetPlayersMode(opt.players_mode)
    .Build();

    TerminalView terminal_view{};

    Controller contoller{model, terminal_view};
    contoller.Run();
}

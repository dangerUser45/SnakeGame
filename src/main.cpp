#include <exception>
#include <getopt.h>

#include "terminal_view.hpp"
#include "controller.hpp"
#include "options.hpp"
#include "model.hpp"

int main(int argc, char** argv)
{
    using namespace snake_game;
    
    try {
        Options opt{};
        opt.GetOptions(argc, argv);

        Model model = Model::Builder()
        .SetWinSize(opt.win_size())
        .SetNumPlayers(opt.num_players())
        .SetNumBots(opt.num_bots())
        .SetRabbPerSnake(opt.rabb_per_snake())
        .SetTicTime(opt.tic_time())
        .Build();

        TerminalView terminal_view{};

        Controller contoller{model, terminal_view};
        contoller.Run();
        
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

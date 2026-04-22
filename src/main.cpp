#include "controller.hpp"
#include "options.hpp"

int main(int argc, char** argv)
{
    using namespace snake_game;
    
    try {
        Options opt{};
        opt.GetOptions(argc, argv);

        Controller controller{opt};
        controller.Run();        
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

#include "options.hpp"
#include <stdexcept>

namespace snake_game {

namespace {
    enum OptionCode { PLAYERS_MODE, NUM_BOTS, RABB_PER_SNAKE, WIN_SIZE };
}

void GetOptions(int argc, char **argv, Options& options)
{
    opterr = 0;
    // TODO вместо следующих нулей что-то поставить
    static struct option long_options[] = {
        {"players_mode",   required_argument, 0, PLAYERS_MODE},
        {"num_bots",       required_argument, 0, NUM_BOTS},
        {"rabb_per_snake", required_argument, 0, RABB_PER_SNAKE},
        {"win_size",       required_argument, 0, WIN_SIZE},
        {0, 0, 0, 0}
    };

    int opt = 0;
    std::string err_message = "";

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case PLAYERS_MODE: {
                std::string_view value = optarg;
                
                if(value == "SINGLE_PLAYER")
                    options.players_mode = Model::PlayersMode::SINGLE_PLAYER;
                else if(value == "TWO_PLAYERS")
                    options.players_mode = Model::PlayersMode::TWO_PLAYERS;
                else 
                    err_message += "Error: mode " + static_cast<std::string>(optarg) + " doesn't exist\n";
                
                break;
            }
        
            case NUM_BOTS: {
                char* endptr = NULL;

                long int num_bots = std::strtol(optarg, &endptr, 10);
                if(*endptr != '\0') 
                    err_message += "Error: you must use only digits\n";

                else if(num_bots < 0 || num_bots > 4) 
                    err_message += "Error: Incorrect numbers of bots. Please, enter number in [0 - 4]\n";

                else options.num_bots = num_bots;
                
                break;
            }

            case RABB_PER_SNAKE: {
                char* endptr = NULL;

                long int rabb_per_snake = std::strtol(optarg, &endptr, 10); 

                if(*endptr != '\0') 
                    err_message += "Error: you must use only digits in '--rab_per_snake' oprtions\n";

                else if(rabb_per_snake < 0 || rabb_per_snake > 10)
                    err_message += "Incorrect numbers of bots. Please, enter number in [0 - 10]\n";

                else options.rabb_per_snake = rabb_per_snake;
                
                break;
            }

            case WIN_SIZE: {
                // char* endptr = NULL;

                // goto 'x'
                // std::string win_weight = optarg
                // long int win_weight = std::strtol(opta) 
                break;
            }

            default: err_message += "Error: unrecognized option '"
                                 + static_cast<std::string>(argv[optind - 1]) + "'\n";
      }
    }

    if(!err_message.empty()) throw std::runtime_error(err_message);
}


} // namespace snake_game
#include "options.hpp"
#include <stdexcept>

namespace snake_game {

namespace {
    enum OptionCode { NUM_PLAYERS, NUM_BOTS, RABB_PER_SNAKE, WIN_SIZE };

    bool IsLongOption(std::string_view arg, std::string_view name)
    {
        if(!arg.starts_with("--"))
            return false;

        arg.remove_prefix(2);
        const std::size_t eq_pos = arg.find('=');
        if(eq_pos != std::string_view::npos)
            arg = arg.substr(0, eq_pos);

        return arg == name;
    }
}

void GetOptions(int argc, char **argv, Options& options)
{
    opterr = 0;
    // TODO вместо следующих нулей что-то поставить
    static struct option long_options[] = {
        {"num_players",    required_argument, 0, NUM_PLAYERS},
        {"num_bots",       required_argument, 0, NUM_BOTS},
        {"rabb_per_snake", required_argument, 0, RABB_PER_SNAKE},
        {"win_size",       required_argument, 0, WIN_SIZE},
        {0, 0, 0, 0}
    };

    int opt = 0;
    std::string err_message = "";

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case NUM_PLAYERS: {
                std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                             : std::string_view{};
                if(value.starts_with("--")) {
                    --optind;
                    err_message += "Error: missing required argument for '--num_players'\n";
                    break;
                }

                char* endptr = NULL;

                long int num_players = std::strtol(optarg, &endptr, 10);
                if(*endptr != '\0') 
                    err_message += "Error: you must use only digits\n";

                else if(num_players < 0 || num_players > 2) 
                    err_message += "Error: Incorrect numbers of players. Please, enter number in [0 - 2]\n";

                else options.num_players = num_players;
                
                break;
            }
        
            case NUM_BOTS: {
                std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                             : std::string_view{};
                if(value.starts_with("--")) {
                    --optind;
                    err_message += "Error: missing required argument for '--num_bots'\n";
                    break;
                }

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
                std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                             : std::string_view{};
                if(value.starts_with("--")) {
                    --optind;
                    err_message += "Error: missing required argument for '--rabb_per_snake'\n";
                    break;
                }

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
                std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                             : std::string_view{};
                if(value.starts_with("--")) {
                    --optind;
                    err_message += "Error: missing required argument for '--win_size'\n";
                    break;
                }

                const std::size_t sep = value.find('x');
                if (sep == std::string_view::npos || sep == 0 || sep == value.size() - 1) {
                    err_message += "Error: '--win_size' must be in format WIDTHxHEIGHT, example:230x123\n";
                    break;
                }

                if (value.find('x', sep + 1) != std::string_view::npos) {
                    err_message += "Error: '--win_size' must contain only one 'x'\n";
                    break;
                }

                int width = 0;
                int height = 0;
                auto parse_num = [&](std::string_view part, const char* what, int& out) {
                    std::string temp(part);
                    char* endptr = nullptr;
                    long result = std::strtol(temp.c_str(), &endptr, 10);

                    if (*endptr != '\0') {
                        err_message += std::string("Error: ") + what + " must contain only digits\n";
                        return false;
                    }

                    if (result <= 0) {
                        err_message += std::string("Error: ") + what + " must be > 0\n";
                        return false;
                    }

                    out = static_cast<int>(result);
                    return true;
                };

                if(parse_num(value.substr(0, sep), "width", width)
                && parse_num(value.substr(sep + 1), "height", height))
                    options.win_size = {width, height};

                break;
            }

            case '?': {
                const std::string_view arg =
                    (optind > 0) ? std::string_view(argv[optind - 1]) : std::string_view{};

                if(IsLongOption(arg, "num_players")
                || IsLongOption(arg, "num_bots")
                || IsLongOption(arg, "rabb_per_snake")
                || IsLongOption(arg, "win_size"))
                    err_message += "Error: missing required argument for '" + std::string(arg) + "'\n";
                else
                    err_message += "Error: unrecognized option '" + std::string(arg) + "'\n";
                break;
            }

            default: err_message += "Error: unrecognized option '"
                                 + static_cast<std::string>(argv[optind - 1]) + "'\n";
      }
    }
    
    if(!err_message.empty()) throw std::runtime_error(err_message);
}


} // namespace snake_game

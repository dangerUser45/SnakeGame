#include "options.hpp"
#include "model.hpp"
#include <stdexcept>

namespace snake_game {

namespace {
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

long int Options::GetNum(long int min, long int max, const std::string& label)
{
    std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                    : std::string_view{};
    if(value.starts_with("--")) {
        --optind;
        err_message_ += static_cast<std::string>("Error: missing required argument for '--")
            + static_cast<std::string>(long_options[0].name) + "'\n";
        return -1;
    }

    char* endptr = NULL;

    long int num = std::strtol(optarg, &endptr, 10);
    if(*endptr != '\0') 
        err_message_ += "Error: you must use only digits\n";

    else if(num < min || num > max) 
        err_message_ += "Error: Incorrect" + label + ". Please, enter number in ["
                    + std::to_string(min) + " - "
                    + std::to_string(max) + "]\n";;

    return num;

}

void Options::GetOptions(int argc, char **argv)
{
    opterr = 0;
    while ((opt_ = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt_) {
            case NUM_PLAYERS:
                num_players_ = GetNum(limits::MIN_NUM_PLAYERS,
                    limits::MAX_NUM_PLAYERS, std::string("number of players"));
                break;
        
            case NUM_BOTS:
                num_bots_ = GetNum(limits::MIN_NUM_BOTS,
                    limits::MAX_NUM_BOTS, std::string("number of bots"));
                break;

            case TIC_TIME:
                tic_time_ = GetNum(limits::MIN_TIC_TIME,
                    limits::MAX_TIC_TIME, std::string("milliseconds per frame"));
                break;

            case RABB_PER_SNAKE:
                rabb_per_snake_ = GetNum(limits::MIN_TIC_TIME,
                    limits::MAX_TIC_TIME, std::string("milliseconds per frame"));
                break;

            case VIEW_MODE: {
                    std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                                 : std::string_view{};
                    if(value.starts_with("--")) {
                        --optind;
                        err_message_ += static_cast<std::string>("Error: missing required argument for '--")
                            + static_cast<std::string>(long_options[0].name) + "'\n";
                        // break;
                    }

                    if(value == "graphical")
                        view_mode_ = ViewMode::GRAPHICAL_VIEW;
                    else if(value == "terminal")
                        view_mode_ = ViewMode::TERMINAL_VIEW;
                    else
                        err_message_ += "Error: Incorrect option. Please, try 'graphical or 'terminal'\n";
                    
                    break;
            }

            case WIN_SIZE: {
                std::string_view value = (optarg != nullptr) ? std::string_view(optarg)
                                                             : std::string_view{};
                if(value.starts_with("--")) {
                    --optind;
                    err_message_ += "Error: missing required argument for '--win_size'\n";
                    break;
                }

                const std::size_t sep = value.find('x');
                if (sep == std::string_view::npos || sep == 0 || sep == value.size() - 1) {
                    err_message_ += "Error: '--win_size' must be in format WIDTHxHEIGHT, example:230x123\n";
                    break;
                }

                if (value.find('x', sep + 1) != std::string_view::npos) {
                    err_message_ += "Error: '--win_size' must contain only one 'x'\n";
                    break;
                }

                int width = 0, height = 0;
                auto parse_num = [&](std::string_view part, const char* what, int& out) {
                    std::string temp(part);
                    char* endptr = nullptr;
                    long result = std::strtol(temp.c_str(), &endptr, 10);

                    if (*endptr != '\0') {
                        err_message_ += std::string("Error: ") + what + " must contain only digits\n";
                        return false;
                    }

                    if (result <= 0) {
                        err_message_ += std::string("Error: ") + what + " must be > 0\n";
                        return false;
                    }

                    out = static_cast<int>(result);
                    return true;
                };

                if(parse_num(value.substr(0, sep), "width", width)
                && parse_num(value.substr(sep + 1), "height", height))
                    win_size_ = {width, height};

                break;
            }

            case '?': {
                const std::string_view arg =
                    (optind > 0) ? std::string_view(argv[optind - 1]) : std::string_view{};

                if(IsLongOption(arg, "num_players")
                || IsLongOption(arg, "num_bots")
                || IsLongOption(arg, "tic_time")
                || IsLongOption(arg, "view")
                || IsLongOption(arg, "rabb_per_snake")
                || IsLongOption(arg, "win_size"))
                    err_message_ += "Error: missing required argument for '" + std::string(arg) + "'\n";
                else
                    err_message_ += "Error: unrecognized option '" + std::string(arg) + "'\n";
                break;
            }

            default: err_message_ += "Error: unrecognized option '"
                                 + static_cast<std::string>(argv[optind - 1]) + "'\n";
      }
    }
    
    if(!err_message_.empty()) throw std::runtime_error(err_message_);
}


} // namespace snake_game

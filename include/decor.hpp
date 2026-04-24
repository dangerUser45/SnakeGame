#pragma once

#include <cstdint>
#include <string_view>

namespace snake_game {
    
    enum class ObjColor : std::uint8_t {
        RED,
        ORANGE,
        GREEN,
        LIGHT_BLUE,
        BLUE, PURPLE,
        WITHOUT_COLOR };

} // namespace snake_game

namespace snake_game::glyphs {

    inline constexpr std::string_view SNAKE_BODY          = "#";
    inline constexpr std::string_view RABBIT_BODY         = "*";
    inline constexpr std::string_view POWER_BOX           = "$";

    inline constexpr std::string_view HEAD_LEFT           = "←";
    inline constexpr std::string_view HEAD_RIGHT          = "→";
    inline constexpr std::string_view HEAD_UP             = "↑";
    inline constexpr std::string_view HEAD_DOWN           = "↓"; 
    
    inline constexpr std::string_view BORDER_HOR          = "─";
    inline constexpr std::string_view BORDER_VERT         = "│";
    inline constexpr std::string_view CORNER_TOP_LEFT     = "╭";
    inline constexpr std::string_view CORNER_TOP_RIGHT    = "╮";
    inline constexpr std::string_view CORNER_BOTTOM_LEFT  = "╰";
    inline constexpr std::string_view CORNER_BOTTOM_RIGHT = "╯";

} // namespace snake_game::glyphs

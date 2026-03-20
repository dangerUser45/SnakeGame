#pragma once

#include "ostream"

namespace snake_game {

enum class Event {
    KEY_PRESSED_P1_LEFT,
    KEY_PRESSED_P1_RIGHT,
    KEY_PRESSED_P1_UP,
    KEY_PRESSED_P1_DOWN,

    KEY_PRESSED_P2_LEFT,
    KEY_PRESSED_P2_RIGHT,
    KEY_PRESSED_P2_UP,
    KEY_PRESSED_P2_DOWN,

    KEY_PRESSED_PAUSE,
    KEY_PRESSED_EXIT,
    RESIZE_WINDOW
};
    
std::ostream& operator<< (std::ostream& os, Event event);
    
} //namespace snake_game
#include "event.hpp"

namespace snake_game {

std::ostream& operator<< (std::ostream& os, Event event)
{
    switch(event) {
        case Event::KEY_PRESSED_P1_LEFT:  return os << "a";             break;
        case Event::KEY_PRESSED_P1_RIGHT: return os << "d";             break;
        case Event::KEY_PRESSED_P1_UP:    return os << "w";             break;
        case Event::KEY_PRESSED_P1_DOWN:  return os << "s";             break;

        case Event::KEY_PRESSED_P2_LEFT:  return os << "←";             break;
        case Event::KEY_PRESSED_P2_RIGHT: return os << "→";             break;
        case Event::KEY_PRESSED_P2_UP:    return os << "↑ ";            break;
        case Event::KEY_PRESSED_P2_DOWN:  return os << "↓";             break;

        case Event::KEY_PRESSED_PAUSE:    return os << "PAUSE";         break;
        case Event::KEY_PRESSED_EXIT:     return os << "EXIT";          break;
        case Event::RESIZE_WINDOW:        return os << "RESIZE WINDOW"; break;

        //TODO обработать 
        default: return os << "Error";
    }
} 

} //namespace snake_game

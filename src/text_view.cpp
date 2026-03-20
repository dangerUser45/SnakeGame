#include <memory>
#include <optional>
#include <iostream>

#include "view.hpp"
#include "decor.hpp"

namespace snake_game {

TextView::TextView() : impl_(std::make_unique<Impl>())
{
    impl_->SetupTerminal();
    impl_->ClearScreen();
    impl_->HideCursor();
}

TextView::~TextView()
{
    impl_->RestoreTerminal();
    impl_->ClearScreen();
    impl_->ShowCursor();
}

void TextView::Impl::GotoXY(Coord coord) const
{
    std::cout << "\033[" << coord.y + 1 << ";" << coord.x + 1 << "H";
}

void TextView::Impl::GotoXY(int32_t x, int32_t y) const
{
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void TextView::Impl::ClearScreen()
{
    std::cout << "\033[H\033[J";
}

void TextView::Impl::DrawSnake(const Snake& snake) const
{
    Coord snake_head = snake.body_.front();
    GotoXY(snake_head);
    std::cout << DrawSnakeHead(snake.dir_);

    for(auto it = snake.body_.cbegin() + 1,
              end = snake.body_.cend(); it != end; ++it) {
        GotoXY(*it);
        std::cout << glyphs::SNAKE_BODY;
    }
}

const std::string_view TextView::Impl::
DrawSnakeHead(Direction dir) const
{
    switch(dir){
        case Direction::LEFT:  return glyphs::HEAD_LEFT;  break;
        case Direction::RIGHT: return glyphs::HEAD_RIGHT; break;
        case Direction::UP:    return glyphs::HEAD_UP;    break;
        case Direction::DOWN:  return glyphs::HEAD_DOWN;  break;
        // TODO добавить default и его обработку
    }
}

void TextView::Impl::DrawRabbit(const Rabbit& rabbit) const
{
    //TODO убрать заглушку
    (void)rabbit;
}

void TextView::Impl::DrawBackground(const Coord win_size) const
{
    int32_t init_x = 0;
    int32_t init_y = 0;

    GotoXY(init_x, init_y);
    std::cout << glyphs::CORNER_TOP_LEFT;
    for(int32_t i = 0; i < win_size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_TOP_RIGHT;

    for(int32_t i = 0; i < win_size.y - 2; ++i) {
        GotoXY(init_x, init_y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
        GotoXY(init_x + win_size.x - 1, init_y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
    }

    GotoXY(init_x, init_y + win_size.y - 1);
    std::cout << glyphs::CORNER_BOTTOM_LEFT;
    for(int32_t i = 0; i < win_size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_BOTTOM_RIGHT;
}

void TextView::Impl::SetupTerminal()
{
    if (is_terminal_configured_) {
        return;
    }

    tcgetattr(STDIN_FILENO, &old_term_);
    termios new_term = old_term_;

    new_term.c_lflag &= ~(ICANON | ECHO); 

    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    is_terminal_configured_ = true;
}

void TextView::Impl::RestoreTerminal()
{
    if (!is_terminal_configured_) {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    is_terminal_configured_ = false;
}

void TextView::Impl::HideCursor() const
{
    std::cout << "\033[?25l" << std::flush;
}

void TextView::Impl::ShowCursor() const
{
    std::cout << "\033[?25h" << std::flush;
}

std::optional<Event> TextView::Impl::PopNextEvent()
{
      if (events_.empty()) {
          return std::nullopt;
      }

      const Event event = events_.front();
      events_.pop();
      return event;
}

void TextView::Impl::UpdateEventsBuffer()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    timeval timeout{0, 0};

    const int ready = ::select(STDIN_FILENO + 1, &readfds, nullptr,
nullptr, &timeout);
    if (ready < 0) { return; }
    if (ready == 0) { return; }
    if (!FD_ISSET(STDIN_FILENO, &readfds)) { return; }

    char buffer[256];
    const ssize_t nread = ::read(STDIN_FILENO, buffer,
sizeof(buffer));
    if (nread <= 0) { return; }

    for (ssize_t i = 0; i < nread; ) {
        const unsigned char ch = static_cast<unsigned
    char>(buffer[i]);

    switch (ch) {
        case 'w':
        case 'W':
            events_.push(Event::KEY_PRESSED_P1_UP);
            ++i;
            break;

        case 's':
        case 'S':
            events_.push(Event::KEY_PRESSED_P1_DOWN);
            ++i;
            break;

        case 'a':
        case 'A':
            events_.push(Event::KEY_PRESSED_P1_LEFT);
            ++i;
            break;

        case 'd':
        case 'D':
            events_.push(Event::KEY_PRESSED_P1_RIGHT);
            ++i;
            break;

        case 'p':
        case 'P':
            events_.push(Event::KEY_PRESSED_PAUSE);
            ++i;
            break;

        case 'q':
        case 'Q':
            events_.push(Event::KEY_PRESSED_EXIT);
            ++i;
            break;

        case '\x1b':
            if (i + 2 < nread && buffer[i + 1] == '[') {
                switch (buffer[i + 2]) {
                case 'A':
                    events_.push(Event::KEY_PRESSED_P2_UP);
                    i += 3;
                    break;
                case 'B':
                    events_.push(Event::KEY_PRESSED_P2_DOWN);
                    i += 3;
                    break;
                case 'C':
                    events_.push(Event::KEY_PRESSED_P2_RIGHT);
                    i += 3;
                    break;
                case 'D':
                    events_.push(Event::KEY_PRESSED_P2_LEFT);
                    i += 3;
                    break;
                default:
                    ++i;
                    break;
                }
            } else {
                ++i;
            }
            break;

        default:
            ++i;
            break;
        }
    }
}

std::optional<Event> TextView::PollEvents()
{
    if (auto event = impl_->PopNextEvent()) {
        return event;
    }

    impl_->UpdateEventsBuffer();
    return impl_->PopNextEvent();
}

void TextView::Render(Model& model)
{
    impl_->ClearScreen();

    impl_->DrawBackground(model.win_size_);

    for(auto&& snake : model.snakes_)
        impl_->DrawSnake(snake);

    for(auto&& rabbit : model.rabbits_)
        impl_->DrawRabbit(rabbit);


    std::cout << std::flush;
}

} // namespace snake_game

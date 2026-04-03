#include <optional>
#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#include "terminal_view.hpp"
#include "decor.hpp"

namespace snake_game {

TerminalView::TerminalView() : impl_(std::make_unique<Impl>())
{
    impl_->SetupTerminal();
    impl_->ClearScreen();
    impl_->HideCursor();
}

TerminalView::~TerminalView()
{
    impl_->RestoreTerminal();
    impl_->ClearScreen();
    impl_->ShowCursor();
}

void TerminalView::Impl::GotoXY(Coord coord) const
{
    std::cout << "\033[" << coord.y + 1 << ";" << coord.x + 1 << "H";
}

void TerminalView::Impl::GotoXY(int x, int y) const
{
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void TerminalView::Impl::ClearScreen()
{
    std::cout << "\033[H\033[J";
}

namespace {
    using namespace std::literals;
    inline constexpr std::array color_array {
        "\033[31m"sv, // Red
        "\033[32m"sv, // Green
        "\033[33m"sv, // Yellow
        "\033[34m"sv, // Blue
        "\033[36m"sv  // Cyan
  };

  inline constexpr std::string_view ResetColor{"\033[0m"};
}

std::string_view TerminalView::Impl::GetTerminalColor(ObjColor color) const
{
    return color_array[static_cast<uint8_t>(color)];
}

void TerminalView::Impl::DrawSnake(const Snake& snake) const
{
    Coord snake_head = snake.body_.front();
    GotoXY(snake_head);
    const std::string_view snake_color = GetTerminalColor(snake.color_);
    std::cout << snake_color << DrawSnakeHead(snake.dir_) << ResetColor;

    for(auto it = snake.body_.cbegin() + 1,
              end = snake.body_.cend(); it != end; ++it) {
        GotoXY(*it);
        std::cout << snake_color << glyphs::SNAKE_BODY << ResetColor;
    }
}

const std::string_view TerminalView::Impl::
DrawSnakeHead(Direction dir) const
{
    switch(dir){
        case Direction::LEFT:  return glyphs::HEAD_LEFT;  break;
        case Direction::RIGHT: return glyphs::HEAD_RIGHT; break;
        case Direction::UP:    return glyphs::HEAD_UP;    break;
        case Direction::DOWN:  return glyphs::HEAD_DOWN;  break;

        default: std::cerr << "Error unknown options" << std::endl;
            std::exit(1); // TODO как-нибудь нормально исправить
    }
}

void TerminalView::Impl::DrawRabbit(const Rabbit& rabbit) const
{
    GotoXY(rabbit.body_);
    std::cout << glyphs::RABBIT_BODY;
}

void TerminalView::Impl::DrawBackground(const Coord win_size) const
{
    int init_x = 0;
    int init_y = 0;

    GotoXY(init_x, init_y);
    std::cout << glyphs::CORNER_TOP_LEFT;
    for(int i = 0; i < win_size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_TOP_RIGHT;

    for(int i = 0; i < win_size.y - 2; ++i) {
        GotoXY(init_x, init_y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
        GotoXY(init_x + win_size.x - 1, init_y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
    }

    GotoXY(init_x, init_y + win_size.y - 1);
    std::cout << glyphs::CORNER_BOTTOM_LEFT;
    for(int i = 0; i < win_size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_BOTTOM_RIGHT;
}

void TerminalView::Impl::SetupTerminal()
{
    tcgetattr(STDIN_FILENO, &old_term_);
    termios new_term = old_term_;

    new_term.c_lflag &= ~(ICANON | ECHO); 

    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void TerminalView::Impl::RestoreTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

void TerminalView::Impl::HideCursor() const
{
    std::cout << "\033[?25l" << std::flush;
}

void TerminalView::Impl::ShowCursor() const
{
    std::cout << "\033[?25h" << std::flush;
}

std::optional<Event> TerminalView::Impl::PopNextEvent()
{
      if (events_.empty()) {
          return std::nullopt;
      }

      const Event event = events_.front();
      events_.pop();
      return event;
}

void TerminalView::Impl::UpdateEventsBuffer()
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

std::optional<Event> TerminalView::PollEvents()
{
    if (auto event = impl_->PopNextEvent()) {
        return event;
    }

    impl_->UpdateEventsBuffer();
    return impl_->PopNextEvent();
}

void TerminalView::Render(Model& model)
{
    // TODO добавить updates
    // impl_->ClearScreen();

    if(impl_->is_init_drawing_) {
        impl_->DrawBackground(model.win_size_);
        impl_->is_init_drawing_ = false;
    }

    for(auto&& snake : model.snakes_)
        impl_->DrawSnake(snake);

    for(auto&& rabbit : model.rabbits_)
        impl_->DrawRabbit(rabbit);

    std::cout << std::flush;
}

} // namespace snake_game

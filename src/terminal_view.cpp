#include <optional>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

#include "limits.hpp"
#include "terminal_view.hpp"
#include "decor.hpp"
#include "model.hpp"

namespace snake_game {

namespace {
    inline constexpr int BannerWidth = 59;
    inline constexpr int BannerHeight = 3;
    inline constexpr int BannerGapY = 2;
    inline constexpr int StatsGapX = 3;
    inline constexpr int StatsWindowWidth = 32;
}

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
    std::cout << "\033[" << coord.y + field_origin_.y << ";"
              << coord.x + field_origin_.x  << "H";
}

void TerminalView::Impl::GotoXY(int x, int y) const
{
    std::cout << "\033[" << y + field_origin_.y << ";"
              << x + field_origin_.x << "H";
}

void TerminalView::Impl::GotoXYInit(int x, int y) const
{
    std::cout << "\033[" << y << ";" << x << "H";    
}

void TerminalView::Impl::ClearScreen() const
{
    std::cout << "\033[H\033[J";
}

namespace {
    using namespace std::string_view_literals;
    inline constexpr std::array color_array {
        "\033[91m"sv, // Red
        "\033[93m"sv, // Orange
        "\033[92m"sv, // Green
        "\033[94m"sv, // Blue
        "\033[96m"sv, // Light blue
        "\033[95m"sv, // Purple
        ""sv,         // Without color
  };

    inline constexpr std::string_view ResetColor{"\033[0m"};
    inline constexpr std::string_view WarningBgColor{"\033[48;5;236m"};
    inline constexpr std::string_view WarningTextColor{"\033[38;5;250m"};
    inline constexpr std::string_view WarningAccentColor{"\033[38;5;203m"};

    std::string GetBotAlgorithmLabel(BotAlgorithm algorithm)
    {
        switch(algorithm) {
            case BotAlgorithm::DUMB:   return "Dumb";
            case BotAlgorithm::MEDIUM: return "Medium";
            case BotAlgorithm::SMARTY: return "Smarty";
            default: return "Unknown";
        }
    }
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

    terminal_size_ = GetTerminalSize();
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

Coord TerminalView::Impl::GetTerminalSize() const
{
    winsize win_size{};
    if(::ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size) == -1)
        return terminal_size_;

    return {static_cast<int>(win_size.ws_col),
            static_cast<int>(win_size.ws_row)};
}

bool TerminalView::Impl::UpdateTerminalSize()
{
    const Coord current_size = GetTerminalSize();
    if(current_size == terminal_size_)
        return false;

    terminal_size_ = current_size;
    is_init_rendering_ = true;
    events_.push(Event::RESIZE_WINDOW);
    return true;
}

void TerminalView::Impl::SyncSnakeStats(const Model& model)
{
    auto get_controller_label = [&](const Snake& snake) {
        for(std::size_t i = 0; i < model.hcontrol_.size(); ++i)
            if(model.hcontrol_[i] == &snake)
                return "Player " + std::to_string(i + 1);

        return GetBotAlgorithmLabel(snake.bot_);
    };

    std::vector<ObjColor> seen_colors{};
    seen_colors.reserve(model.snakes_.size());

    for(const auto& snake : model.snakes_) {
        auto it = std::find_if(snake_stats_.begin(), snake_stats_.end(),
            [&](const SnakeStatsEntry& entry) {
                return entry.color_ == snake.color_;
            });

        if(it == snake_stats_.end()) {
            snake_stats_.push_back({
                snake.color_,
                get_controller_label(snake),
                snake.dir_,
                static_cast<int>(snake.body_.size()),
                snake.kill_num,
                snake.is_live_
            });
            it = snake_stats_.end() - 1;
        }

        it->controller_label_ = get_controller_label(snake);
        it->dir_ = snake.dir_;
        it->score_ = static_cast<int>(snake.body_.size());
        it->kills_ = snake.kill_num;
        it->is_live_ = snake.is_live_;
        seen_colors.push_back(snake.color_);
    }

    for(auto& entry : snake_stats_)
        if(std::find(seen_colors.begin(), seen_colors.end(), entry.color_) == seen_colors.end())
            entry.is_live_ = false;
}

Coord TerminalView::Impl::GetStatsWindowSize(const Model& model) const
{
    const int snake_count = std::max(static_cast<int>(snake_stats_.size()),
                                     model.num_players_ + model.num_bots_);
    return {
        StatsWindowWidth,
        std::max(5, 3 + snake_count * 6)
    };
}

Coord TerminalView::Impl::GetContentSize(const Model& model) const
{
    const Coord field_size = model.win_size_;
    const Coord stats_size = GetStatsWindowSize(model);
    return {
        std::max(field_size.x + StatsGapX + stats_size.x, BannerWidth),
        BannerHeight + BannerGapY + std::max(field_size.y, stats_size.y)
    };
}

void TerminalView::Impl::UpdateLayout(const Model& model)
{
    const Coord content_size = GetContentSize(model);
    const Coord stats_size = GetStatsWindowSize(model);
    const int body_width = model.win_size_.x + StatsGapX + stats_size.x;
    const Coord content_origin{
        std::max(1, (terminal_size_.x - content_size.x) / 2 + 1),
        std::max(1, (terminal_size_.y - content_size.y) / 2 + 1)
    };
    const Coord body_origin{
        content_origin.x + (content_size.x - body_width) / 2,
        content_origin.y + BannerHeight + BannerGapY
    };

    banner_origin_ = {
        content_origin.x + (content_size.x - BannerWidth) / 2,
        content_origin.y
    };

    field_origin_ = {
        body_origin.x,
        body_origin.y
    };

    stats_origin_ = {
        body_origin.x + model.win_size_.x + StatsGapX,
        body_origin.y
    };
}

Coord TerminalView::Impl::GetRequiredTerminalSize(const Model& model) const
{
    return GetContentSize(model);
}

Coord TerminalView::Impl::GetBotChampionshipSize(const BotChampionshipStats& stats) const
{
    return {
        78,
        BannerHeight + BannerGapY + 7 + static_cast<int>(stats.entries_.size()) * 3
    };
}

Coord TerminalView::Impl::GetMaxSupportedTerminalSize() const
{
    const Coord stats_size = {
        StatsWindowWidth,
        std::max(5, 3 + static_cast<int>(limits::MAX_NUM_PLAYERS
                      + limits::MAX_NUM_BOTS) * 6)
    };
    return {
        std::max(static_cast<int>(limits::MAX_WIN_WIDTH) + StatsGapX + stats_size.x,
                 BannerWidth),
        BannerHeight + BannerGapY
            + std::max(static_cast<int>(limits::MAX_WIN_HEIGHT), stats_size.y)
    };
}

void TerminalView::Impl::DrawFrame(Coord origin, Coord size) const
{
    GotoXYInit(origin.x, origin.y);
    std::cout << glyphs::CORNER_TOP_LEFT;
    for(int i = 0; i < size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_TOP_RIGHT;

    for(int i = 0; i < size.y - 2; ++i) {
        GotoXYInit(origin.x, origin.y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
        GotoXYInit(origin.x + size.x - 1, origin.y + 1 + i);
        std::cout << glyphs::BORDER_VERT;
    }

    GotoXYInit(origin.x, origin.y + size.y - 1);
    std::cout << glyphs::CORNER_BOTTOM_LEFT;
    for(int i = 0; i < size.x - 2; ++i)
        std::cout << glyphs::BORDER_HOR;
    std::cout << glyphs::CORNER_BOTTOM_RIGHT;
}

void TerminalView::Impl::DrawBotChampionship(const BotChampionshipStats& stats)
{
    ClearScreen();

    const Coord content_size = GetBotChampionshipSize(stats);
    const Coord content_origin{
        std::max(1, (terminal_size_.x - content_size.x) / 2 + 1),
        std::max(1, (terminal_size_.y - content_size.y) / 2 + 1)
    };
    const Coord frame_size{
        content_size.x,
        content_size.y - BannerHeight - BannerGapY
    };
    const Coord frame_origin{
        content_origin.x,
        content_origin.y + BannerHeight + BannerGapY
    };
    const int inner_width = frame_size.x - 2;

    auto draw_text = [&](int x, int y, const std::string& text) {
        if(x >= inner_width)
            return;

        GotoXYInit(frame_origin.x + 1 + x, frame_origin.y + 1 + y);
        std::cout << text.substr(0, inner_width - x);
    };

    banner_origin_ = {
        content_origin.x + (content_size.x - BannerWidth) / 2,
        content_origin.y
    };

    DrawBanner();
    DrawFrame(frame_origin, frame_size);

    for(int y = 1; y < frame_size.y - 1; ++y) {
        GotoXYInit(frame_origin.x + 1, frame_origin.y + y);
        std::cout << std::string(inner_width, ' ');
    }

    const std::string title = "Bot Championship";
    draw_text(std::max(0, (inner_width - static_cast<int>(title.size())) / 2), 0, title);
    draw_text(2, 2, "Rounds: " + std::to_string(stats.rounds_)
                 + "  Draws: " + std::to_string(stats.draws_)
                 + "  Timeouts: " + std::to_string(stats.timeouts_));

    for(std::size_t i = 0; i < stats.entries_.size(); ++i) {
        const auto& entry = stats.entries_[i];
        const int row = 5 + static_cast<int>(i) * 3;

        GotoXYInit(frame_origin.x + 3, frame_origin.y + 1 + row);
        std::cout << GetTerminalColor(entry.color_)
                  << glyphs::SNAKE_BODY
                  << glyphs::SNAKE_BODY
                  << DrawSnakeHead(Direction::RIGHT)
                  << ResetColor;

        draw_text(8, row, entry.label_
            + "  Wins: " + std::to_string(entry.wins_)
            + "  Survived: " + std::to_string(entry.survived_rounds_));
        draw_text(8, row + 1, "Kills: " + std::to_string(entry.total_kills_));
    }
}

void TerminalView::Impl::DrawStatsWindow(const Model& model) const
{
    const Coord stats_size = GetStatsWindowSize(model);
    const int inner_width = stats_size.x - 2;

    DrawFrame(stats_origin_, stats_size);

    for(int y = 1; y < stats_size.y - 1; ++y) {
        GotoXYInit(stats_origin_.x + 1, stats_origin_.y + y);
        std::cout << std::string(inner_width, ' ');
    }

    auto draw_text = [&](int x, int y, const std::string& text) {
        if(x >= inner_width)
            return;

        GotoXYInit(stats_origin_.x + 1 + x, stats_origin_.y + 1 + y);
        std::cout << text.substr(0, inner_width - x);
    };

    auto draw_colored_text = [&](int x, int y, std::string_view text,
                                 ObjColor color) {
        if(x >= inner_width)
            return;

        GotoXYInit(stats_origin_.x + 1 + x, stats_origin_.y + 1 + y);
        std::cout << GetTerminalColor(color)
                  << text.substr(0, inner_width - x)
                  << ResetColor;
    };

    const std::string title = "Statistics";
    draw_text(std::max(0, (inner_width - static_cast<int>(title.size())) / 2),
              0, title);

    for(std::size_t i = 0; i < snake_stats_.size(); ++i) {
        const auto& entry = snake_stats_[i];
        const int row = 2 + static_cast<int>(i) * 6;
        const std::string label = "Snake " + std::to_string(i + 1) + ": ";

        draw_text(0, row, label);
        GotoXYInit(stats_origin_.x + 1 + static_cast<int>(label.size()),
                   stats_origin_.y + 1 + row);
        std::cout << GetTerminalColor(entry.color_)
                  << glyphs::SNAKE_BODY
                  << glyphs::SNAKE_BODY
                  << DrawSnakeHead(Direction::RIGHT)
                  << ResetColor;

        draw_text(2, row + 1, "Status: ");
        if(entry.is_live_)
            draw_text(10, row + 1, "Alive");
        else
            draw_colored_text(10, row + 1, "Dead", ObjColor::RED);

        draw_text(2, row + 2, "Score : " + std::to_string(entry.score_));

        draw_text(2, row + 3, "Kills : " + std::to_string(entry.kills_));

        draw_text(2, row + 4, "Type  : " + entry.controller_label_);
    }
}

void TerminalView::Impl::DrawViewportWarning(const Model& model) const
{
    ClearScreen();

    const Coord current_size = terminal_size_;
    const Coord required_size = GetRequiredTerminalSize(model);
    const Coord max_supported_size = GetMaxSupportedTerminalSize();
    const bool is_too_small = current_size.x < required_size.x
                           || current_size.y < required_size.y;

    auto get_centered_x = [&](const std::string& line) {
        return std::max(1, (terminal_size_.x - static_cast<int>(line.size())) / 2 + 1);
    };

    auto draw_label = [&](int y, const std::string& line) {
        GotoXYInit(get_centered_x(line), y);
        std::cout << WarningBgColor << WarningTextColor
                  << line << ResetColor;
    };

    auto draw_size_line = [&](int y, Coord size) {
        const std::string line = "Width = " + std::to_string(size.x)
                               + " Height = " + std::to_string(size.y);
        GotoXYInit(get_centered_x(line), y);
        std::cout << WarningBgColor
                  << WarningTextColor << "Width = "
                  << WarningAccentColor << size.x
                  << WarningTextColor << " Height = "
                  << WarningAccentColor << size.y
                  << ResetColor;
    };

    const int top = std::max(2, terminal_size_.y / 2 - 3);
    if(is_too_small) {
        draw_label(top, "Terminal size too small:");
        draw_size_line(top + 1, current_size);
        draw_label(top + 3, "Needed for current config:");
        draw_size_line(top + 4, required_size);
    }
    else {
        draw_label(top, "Terminal size exceeds limits.hpp:");
        draw_size_line(top + 1, current_size);
        draw_label(top + 3, "Maximum supported window:");
        draw_size_line(top + 4, max_supported_size);
    }
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
            events_.push(Event::KEY_PRESSED_EXIT);
            ++i;
            break;

        case '\033':
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
    impl_->UpdateTerminalSize();

    if (auto event = impl_->PopNextEvent()) {
        return event;
    }

    impl_->UpdateEventsBuffer();
    return impl_->PopNextEvent();
}

bool TerminalView::CanRenderGameplay(const Model& model) const
{
    const Coord terminal_size = impl_->GetTerminalSize();
    const Coord required_size = impl_->GetRequiredTerminalSize(model);
    if(terminal_size.x < required_size.x
    || terminal_size.y < required_size.y)
        return false;

    const Coord max_supported_size = impl_->GetMaxSupportedTerminalSize();
    if(terminal_size.x > max_supported_size.x
    || terminal_size.y > max_supported_size.y)
        return false;

    return true;
}

void TerminalView::Impl::FullRender(Model& model)
{
    SyncSnakeStats(model);
    UpdateLayout(model);
    ClearScreen();
    DrawBanner();
    DrawBackground(model.win_size_);
    DrawStatsWindow(model);

    for(auto&& snake : model.snakes_)
        DrawSnake(snake);

    for(auto&& rabbit : model.rabbits_)
        DrawRabbit(rabbit);
}

void TerminalView::Impl::DrawBanner() const
{
    Coord init = banner_origin_;

    GotoXYInit(init.x, init.y);
    std::cout << "▄█████ ▄▄  ▄▄  ▄▄▄  ▄▄ ▄▄ ▄▄▄▄▄  ▄████   ▄▄▄  ▄▄   ▄▄ ▄▄▄▄▄\n";
    
    GotoXYInit(init.x, init.y + 1);
    std::cout << "▀▀▀▄▄▄ ███▄██ ██▀██ ██▄█▀ ██▄▄  ██  ▄▄▄ ██▀██ ██▀▄▀██ ██▄▄ \n";
    
    GotoXYInit(init.x, init.y + 2);
    std::cout << "█████▀ ██ ▀██ ██▀██ ██ ██ ██▄▄▄  ▀███▀  ██▀██ ██   ██ ██▄▄▄\n";
}

const std::string_view TerminalView::Impl::DrawUpdate(Model::Updates& update)
{
    switch(update.upd_kind_) {
        case Model::UpdKind::EMPTY:      return " ";
        case Model::UpdKind::SNAKE_BODY: return glyphs::SNAKE_BODY;
        case Model::UpdKind::SNAKE_HEAD: return DrawSnakeHead(update.dir_);
        case Model::UpdKind::RABBIT:     return glyphs::RABBIT_BODY;
        case Model::UpdKind::POWER:      return glyphs::POWER_BOX;

        default: throw std::runtime_error("Error: uknwon kind of updates\n");
    }
}

void TerminalView::Impl::UpdatesRender(Model&  model)
{
    SyncSnakeStats(model);
    UpdateLayout(model);
    DrawStatsWindow(model);
    for(auto&& update : model.updates_) {
        GotoXY(update.coord_);
        std::cout << GetTerminalColor(update.color_)
                  << DrawUpdate(update) << ResetColor;
    }
}

void TerminalView::Render(Model& model)
{
    if(!CanRenderGameplay(model)) {
        impl_->DrawViewportWarning(model);
        std::cout << std::flush;
        return;
    }

    if(impl_->is_init_rendering_) {
        impl_->FullRender(model);
        impl_->is_init_rendering_ = false;
    }
    else
        impl_->UpdatesRender(model);

    std::cout << std::flush;
}

void TerminalView::RenderBotChampionship(const BotChampionshipStats& stats)
{
    impl_->UpdateTerminalSize();

    const Coord required_size = impl_->GetBotChampionshipSize(stats);
    const Coord terminal_size = impl_->GetTerminalSize();
    if(terminal_size.x < required_size.x
    || terminal_size.y < required_size.y) {
        impl_->ClearScreen();
        const std::string line = "Enlarge terminal to view bot championship results";
        impl_->GotoXYInit(
            std::max(1, (terminal_size.x - static_cast<int>(line.size())) / 2 + 1),
            std::max(2, terminal_size.y / 2));
        std::cout << line << std::flush;
        return;
    }

    impl_->DrawBotChampionship(stats);
    std::cout << std::flush;
}

} // namespace snake_game

#include "graphical_view.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <SFML/Graphics.hpp>

namespace snake_game {

namespace {

inline constexpr char FontPath[] = "preset/font/geist_bold.ttf";
inline constexpr char BackgroundPath[] = "preset/img/background.jpg";

inline constexpr float OuterPadding = 28.f;
inline constexpr float TitleGapY = 18.f;
inline constexpr float StatsGapX = 18.f;
inline constexpr float MinCellSize = 4.f;
inline constexpr float MaxCellSize = 24.f;
inline constexpr std::size_t CornerPointCount = 6;

sf::Color GetSnakeColor(ObjColor color)
{
    switch(color) {
        case ObjColor::RED:        return {239, 83, 80};
        case ObjColor::ORANGE:     return {255, 167, 38};
        case ObjColor::GREEN:      return {102, 187, 106};
        case ObjColor::LIGHT_BLUE: return {79, 195, 247};
        case ObjColor::BLUE:       return {66, 165, 245};
        case ObjColor::PURPLE:     return {171, 71, 188};
        case ObjColor::WITHOUT_COLOR:
        default:                   return {189, 189, 189};
    }
}

sf::Color ShiftColor(sf::Color color, int delta)
{
    const auto shift = [delta](std::uint8_t channel) {
        return static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(channel) + delta, 0, 255));
    };

    return {shift(color.r), shift(color.g), shift(color.b), color.a};
}

std::string GetBotAlgorithmLabel(BotAlgorithm algorithm)
{
    switch(algorithm) {
        case BotAlgorithm::DUMB:   return "Dumb";
        case BotAlgorithm::MEDIUM: return "Medium";
        case BotAlgorithm::SMARTY: return "Smarty";
        default:                   return "Unknown";
    }
}

sf::Vector2f RoundVector(sf::Vector2f value)
{
    return {std::round(value.x), std::round(value.y)};
}

sf::ConvexShape MakeRoundedRect(sf::Vector2f size, float radius)
{
    radius = std::max(0.f, std::min(radius, std::min(size.x, size.y) / 2.f));

    sf::ConvexShape shape;
    shape.setPointCount(CornerPointCount * 4);

    const auto fill_corner = [&](std::size_t start, sf::Vector2f center, float start_angle) {
        constexpr float Pi = 3.14159265358979323846f;
        for(std::size_t i = 0; i < CornerPointCount; ++i) {
            const float angle_deg =
                start_angle + 90.f * static_cast<float>(i) / static_cast<float>(CornerPointCount - 1);
            const float angle = angle_deg * Pi / 180.f;
            shape.setPoint(start + i,
                           {center.x + std::cos(angle) * radius,
                            center.y + std::sin(angle) * radius});
        }
    };

    fill_corner(0, {size.x - radius, radius}, -90.f);
    fill_corner(CornerPointCount, {size.x - radius, size.y - radius}, 0.f);
    fill_corner(CornerPointCount * 2, {radius, size.y - radius}, 90.f);
    fill_corner(CornerPointCount * 3, {radius, radius}, 180.f);

    return shape;
}

} // namespace

struct GraphicalView::Impl final {
    sf::RenderWindow window_{};
    sf::Font font_{};
    sf::Texture background_texture_{};
    std::vector<SnakeStatsEntry> snake_stats_{};

    bool is_window_created_ = false;
    bool has_font_ = false;
    bool has_background_ = false;

    void EnsureWindow(sf::Vector2u initial_size)
    {
        if(is_window_created_)
            return;

        window_.create(sf::VideoMode(initial_size),
                       "SnakeGame",
                       sf::Style::Default);
        window_.setVerticalSyncEnabled(true);
        window_.setFramerateLimit(60);
        UpdateView(window_.getSize());
        LoadAssets();
        is_window_created_ = true;
    }

    void EnsureWindow(const Model& model)
    {
        EnsureWindow(GetInitialWindowSize(model));
    }

    void EnsureSummaryWindow()
    {
        EnsureWindow({1100u, 760u});
    }

    [[nodiscard]] sf::Vector2u GetInitialWindowSize(const Model& model) const
    {
        const auto desktop = sf::VideoMode::getDesktopMode().size;
        const auto inner_width = static_cast<unsigned int>(std::max(1, model.win_size_.x - 2));
        const auto inner_height = static_cast<unsigned int>(std::max(1, model.win_size_.y - 2));

        const unsigned int preferred_width = inner_width * 16u + 420u;
        const unsigned int preferred_height = inner_height * 16u + 180u;

        const unsigned int max_width = (desktop.x > 100u) ? desktop.x - 100u : desktop.x;
        const unsigned int max_height = (desktop.y > 120u) ? desktop.y - 120u : desktop.y;

        return {
            std::clamp(preferred_width, 960u, std::max(960u, max_width)),
            std::clamp(preferred_height, 720u, std::max(720u, max_height))
        };
    }

    void LoadAssets()
    {
        has_font_ = font_.openFromFile(FontPath);
        has_background_ = background_texture_.loadFromFile(BackgroundPath);
        if(has_background_)
            background_texture_.setSmooth(true);
    }

    void UpdateView(sf::Vector2u size)
    {
        const sf::Vector2f view_size{
            static_cast<float>(std::max(1u, size.x)),
            static_cast<float>(std::max(1u, size.y))
        };
        window_.setView(sf::View({view_size.x / 2.f, view_size.y / 2.f}, view_size));
    }

    void SyncSnakeStats(const Model& model)
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
                    static_cast<int>(snake.body_.size()),
                    snake.kill_num,
                    snake.is_live_
                });
                it = snake_stats_.end() - 1;
            }

            it->controller_label_ = get_controller_label(snake);
            it->score_ = static_cast<int>(snake.body_.size());
            it->kills_ = snake.kill_num;
            it->is_live_ = snake.is_live_;
            seen_colors.push_back(snake.color_);
        }

        for(auto& entry : snake_stats_)
            if(std::find(seen_colors.begin(), seen_colors.end(), entry.color_) == seen_colors.end())
                entry.is_live_ = false;
    }

    [[nodiscard]] Layout GetLayout(const Model& model) const
    {
        Layout layout{};
        layout.inner_field_size_ = {
            std::max(1, model.win_size_.x - 2),
            std::max(1, model.win_size_.y - 2)
        };

        const sf::Vector2u window_size = window_.getSize();
        const unsigned int min_dimension = std::min(window_size.x, window_size.y);

        layout.title_char_size_ = std::clamp(min_dimension / 12u, 36u, 62u);
        layout.body_char_size_ = std::clamp(min_dimension / 42u, 14u, 22u);
        layout.small_char_size_ = std::max(12u, layout.body_char_size_ - 2u);
        layout.field_padding_ = std::clamp(static_cast<float>(min_dimension) / 220.f, 3.f, 6.f);
        layout.stats_padding_ = std::clamp(static_cast<float>(layout.body_char_size_), 16.f, 24.f);
        layout.panel_radius_ = std::clamp(static_cast<float>(layout.body_char_size_) * 0.85f, 12.f, 18.f);
        layout.outline_thickness_ = 2.f;
        layout.preview_cell_ =
            std::clamp(static_cast<float>(layout.small_char_size_) * 1.05f, 14.f, 18.f);
        layout.stats_width_ =
            std::clamp(static_cast<float>(window_size.x) * 0.28f, 250.f, 320.f);
        layout.title_height_ =
            std::clamp(static_cast<float>(layout.title_char_size_) * 1.15f, 64.f, 96.f);

        const float available_width =
            std::max(1.f, static_cast<float>(window_size.x) - 2.f * OuterPadding
                             - StatsGapX - layout.stats_width_ - 2.f * layout.field_padding_);
        const float available_height =
            std::max(1.f, static_cast<float>(window_size.y) - 2.f * OuterPadding
                             - layout.title_height_ - TitleGapY - 2.f * layout.field_padding_);

        layout.cell_size_ = std::clamp(
            std::min(available_width / static_cast<float>(layout.inner_field_size_.x),
                     available_height / static_cast<float>(layout.inner_field_size_.y)),
            MinCellSize,
            MaxCellSize);

        const sf::Vector2f field_inner_size{
            layout.cell_size_ * static_cast<float>(layout.inner_field_size_.x),
            layout.cell_size_ * static_cast<float>(layout.inner_field_size_.y)
        };

        layout.field_panel_size_ = {
            field_inner_size.x + 2.f * layout.field_padding_,
            field_inner_size.y + 2.f * layout.field_padding_
        };

        const int snake_count = std::max(static_cast<int>(snake_stats_.size()),
                                         model.num_players_ + model.num_bots_);
        const float line_step = static_cast<float>(layout.small_char_size_) * 1.45f;
        layout.entry_height_ =
            static_cast<float>(layout.body_char_size_) + 16.f + 4.f * line_step + 14.f;

        const float stats_content_height =
            2.f * layout.stats_padding_
            + static_cast<float>(layout.body_char_size_) * 1.8f
            + static_cast<float>(snake_count) * layout.entry_height_;

        layout.body_width_ = layout.field_panel_size_.x + StatsGapX + layout.stats_width_;
        layout.body_height_ = std::max(layout.field_panel_size_.y, stats_content_height);

        const float total_height = layout.title_height_ + TitleGapY + layout.body_height_;
        const float top =
            std::max(12.f, (static_cast<float>(window_size.y) - total_height) / 2.f);
        const float left =
            std::max(12.f, (static_cast<float>(window_size.x) - layout.body_width_) / 2.f);

        layout.title_center_ = {
            static_cast<float>(window_size.x) / 2.f,
            top + layout.title_height_ / 2.f
        };

        layout.body_origin_ = {left, top + layout.title_height_ + TitleGapY};

        layout.field_panel_origin_ = {
            left,
            layout.body_origin_.y + (layout.body_height_ - layout.field_panel_size_.y) / 2.f
        };
        layout.field_inner_origin_ = {
            layout.field_panel_origin_.x + layout.field_padding_,
            layout.field_panel_origin_.y + layout.field_padding_
        };

        layout.stats_panel_origin_ = {
            left + layout.field_panel_size_.x + StatsGapX,
            layout.body_origin_.y
        };
        layout.stats_panel_size_ = {layout.stats_width_, layout.body_height_};

        return layout;
    }

    [[nodiscard]] sf::Vector2f CellPosition(Coord coord, const Layout& layout) const
    {
        return {
            layout.field_inner_origin_.x + static_cast<float>(coord.x - 1) * layout.cell_size_,
            layout.field_inner_origin_.y + static_cast<float>(coord.y - 1) * layout.cell_size_
        };
    }

    void DrawBackground()
    {
        if(has_background_) {
            sf::Sprite background(background_texture_);
            const sf::Vector2u texture_size = background_texture_.getSize();
            const sf::Vector2u window_size = window_.getSize();
            background.setScale({
                static_cast<float>(window_size.x) / static_cast<float>(std::max(1u, texture_size.x)),
                static_cast<float>(window_size.y) / static_cast<float>(std::max(1u, texture_size.y))
            });
            window_.draw(background);
        }

        sf::RectangleShape overlay({
            static_cast<float>(window_.getSize().x),
            static_cast<float>(window_.getSize().y)
        });
        overlay.setFillColor({8, 10, 16, 110});
        window_.draw(overlay);
    }

    void DrawPanel(sf::Vector2f origin,
                   sf::Vector2f size,
                   float radius,
                   float outline_thickness)
    {
        origin = RoundVector(origin);
        size = RoundVector(size);

        sf::ConvexShape panel = MakeRoundedRect(size, radius);
        panel.setPosition(origin);
        panel.setFillColor({8, 10, 16, 180});
        panel.setOutlineThickness(-outline_thickness);
        panel.setOutlineColor({255, 255, 255, 235});
        window_.draw(panel);
    }

    void DrawTopLeftText(std::string_view text,
                         unsigned int char_size,
                         sf::Vector2f top_left,
                         sf::Color color,
                         bool bold = false)
    {
        if(!has_font_)
            return;

        sf::Text drawable(font_, std::string(text), char_size);
        drawable.setFillColor(color);
        if(bold)
            drawable.setStyle(sf::Text::Bold);

        const sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition({
            std::round(top_left.x - bounds.position.x),
            std::round(top_left.y - bounds.position.y)
        });
        window_.draw(drawable);
    }

    void DrawCenteredText(std::string_view text,
                          unsigned int char_size,
                          sf::Vector2f center,
                          sf::Color color,
                          bool bold = false)
    {
        if(!has_font_)
            return;

        sf::Text drawable(font_, std::string(text), char_size);
        drawable.setFillColor(color);
        if(bold)
            drawable.setStyle(sf::Text::Bold);

        const sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition({
            std::round(center.x - bounds.position.x - bounds.size.x / 2.f),
            std::round(center.y - bounds.position.y - bounds.size.y / 2.f)
        });
        window_.draw(drawable);
    }

    [[nodiscard]] float MeasureTextWidth(std::string_view text,
                                         unsigned int char_size,
                                         bool bold = false) const
    {
        if(!has_font_)
            return 0.f;

        sf::Text drawable(font_, std::string(text), char_size);
        if(bold)
            drawable.setStyle(sf::Text::Bold);

        return drawable.getLocalBounds().size.x;
    }

    void DrawTitle(const Layout& layout)
    {
        DrawCenteredText("SnakeGame",
                         layout.title_char_size_,
                         layout.title_center_,
                         {255, 255, 255},
                         true);
    }

    void DrawSnakeBlock(sf::Vector2f position,
                        float size,
                        sf::Color fill_color,
                        sf::Color outline_color)
    {
        sf::RectangleShape block({size, size});
        block.setPosition(RoundVector(position));
        block.setFillColor(fill_color);
        if(size >= 10.f) {
            block.setOutlineThickness(-1.f);
            block.setOutlineColor(outline_color);
        }
        window_.draw(block);
    }

    void DrawSnake(const Snake& snake, const Layout& layout)
    {
        const sf::Color body_color = GetSnakeColor(snake.color_);
        const sf::Color head_color = ShiftColor(body_color, 18);
        const sf::Color outline_color = ShiftColor(body_color, -70);
        const float margin = std::max(1.f, layout.cell_size_ * 0.08f);
        const float block_size = std::max(1.f, layout.cell_size_ - 2.f * margin);

        for(std::size_t i = snake.body_.size(); i-- > 0; ) {
            const sf::Vector2f cell = CellPosition(snake.body_[i], layout);
            DrawSnakeBlock({cell.x + margin, cell.y + margin},
                           block_size,
                           (i == 0) ? head_color : body_color,
                           outline_color);
        }
    }

    void DrawRabbit(const Rabbit& rabbit, const Layout& layout)
    {
        const sf::Vector2f cell = CellPosition(rabbit.body_, layout);
        const float radius = std::max(1.f, layout.cell_size_ * 0.22f);

        sf::CircleShape body(radius, 24);
        body.setPosition({
            std::round(cell.x + layout.cell_size_ * 0.28f),
            std::round(cell.y + layout.cell_size_ * 0.28f)
        });
        body.setFillColor({255, 214, 102});
        if(layout.cell_size_ >= 10.f) {
            body.setOutlineThickness(-1.f);
            body.setOutlineColor({173, 121, 39});
        }
        window_.draw(body);
    }

    void DrawStatsPreview(sf::Vector2f origin, float cell_size, sf::Color color)
    {
        const sf::Color head_color = ShiftColor(color, 18);
        const sf::Color outline_color = ShiftColor(color, -70);
        const float margin = std::max(1.f, cell_size * 0.1f);
        const float block_size = std::max(1.f, cell_size - 2.f * margin);

        DrawSnakeBlock({origin.x + margin, origin.y + margin},
                       block_size,
                       color,
                       outline_color);
        DrawSnakeBlock({origin.x + cell_size + margin, origin.y + margin},
                       block_size,
                       color,
                       outline_color);
        DrawSnakeBlock({origin.x + 2.f * cell_size + margin, origin.y + margin},
                       block_size,
                       head_color,
                       outline_color);
    }

    void DrawStatsWindow(const Layout& layout)
    {
        DrawPanel(layout.stats_panel_origin_,
                  layout.stats_panel_size_,
                  layout.panel_radius_,
                  layout.outline_thickness_);

        const float left = layout.stats_panel_origin_.x + layout.stats_padding_;
        const float top = layout.stats_panel_origin_.y + layout.stats_padding_;
        const float right = layout.stats_panel_origin_.x + layout.stats_panel_size_.x - layout.stats_padding_;
        const float line_step = static_cast<float>(layout.small_char_size_) * 1.45f;

        DrawCenteredText("Statistics",
                         layout.body_char_size_,
                         {layout.stats_panel_origin_.x + layout.stats_panel_size_.x / 2.f,
                          top + static_cast<float>(layout.body_char_size_) * 0.65f},
                         {255, 255, 255},
                         true);

        const float divider_y = top + static_cast<float>(layout.body_char_size_) * 1.7f;
        sf::RectangleShape divider({layout.stats_panel_size_.x - 2.f * layout.stats_padding_, 1.f});
        divider.setPosition({std::round(left), std::round(divider_y)});
        divider.setFillColor({255, 255, 255, 90});
        window_.draw(divider);

        const float status_value_offset =
            MeasureTextWidth("Status: ", layout.small_char_size_) + 4.f;
        const float content_top = divider_y + 16.f;

        for(std::size_t i = 0; i < snake_stats_.size(); ++i) {
            const auto& entry = snake_stats_[i];
            const float entry_top = content_top + static_cast<float>(i) * layout.entry_height_;
            const float preview_width = layout.preview_cell_ * 3.f;
            const float preview_x = right - preview_width;

            DrawTopLeftText("Snake " + std::to_string(i + 1),
                            layout.body_char_size_,
                            {left, entry_top},
                            {255, 255, 255},
                            true);

            DrawStatsPreview({preview_x, entry_top + 2.f},
                             layout.preview_cell_,
                             GetSnakeColor(entry.color_));

            const float info_top = entry_top + static_cast<float>(layout.body_char_size_) + 12.f;

            DrawTopLeftText("Status: ",
                            layout.small_char_size_,
                            {left, info_top},
                            {217, 223, 235});
            DrawTopLeftText(entry.is_live_ ? "Alive" : "Dead",
                            layout.small_char_size_,
                            {left + status_value_offset, info_top},
                            entry.is_live_ ? sf::Color{255, 255, 255}
                                           : sf::Color{239, 83, 80},
                            !entry.is_live_);

            DrawTopLeftText("Score: " + std::to_string(entry.score_),
                            layout.small_char_size_,
                            {left, info_top + line_step},
                            {255, 255, 255});
            DrawTopLeftText("Kills: " + std::to_string(entry.kills_),
                            layout.small_char_size_,
                            {left, info_top + 2.f * line_step},
                            {255, 255, 255});
            DrawTopLeftText("Type: " + entry.controller_label_,
                            layout.small_char_size_,
                            {left, info_top + 3.f * line_step},
                            {255, 255, 255});

            if(i + 1 < snake_stats_.size()) {
                sf::RectangleShape separator({
                    layout.stats_panel_size_.x - 2.f * layout.stats_padding_,
                    1.f
                });
                separator.setPosition({
                    std::round(left),
                    std::round(entry_top + layout.entry_height_ - 8.f)
                });
                separator.setFillColor({255, 255, 255, 36});
                window_.draw(separator);
            }
        }
    }

    void DrawBotChampionship(const BotChampionshipStats& stats)
    {
        const sf::Vector2u window_size = window_.getSize();
        const unsigned int min_dimension = std::min(window_size.x, window_size.y);
        const unsigned int title_size = std::clamp(min_dimension / 11u, 34u, 58u);
        const unsigned int body_size = std::clamp(min_dimension / 32u, 16u, 24u);
        const unsigned int small_size = std::max(13u, body_size - 2u);
        const float panel_width =
            std::clamp(static_cast<float>(window_size.x) * 0.72f, 620.f, 900.f);
        const float panel_height =
            std::clamp(210.f + static_cast<float>(stats.entries_.size()) * 110.f, 420.f, 620.f);
        const sf::Vector2f panel_origin = {
            (static_cast<float>(window_size.x) - panel_width) / 2.f,
            (static_cast<float>(window_size.y) - panel_height) / 2.f + 40.f
        };
        const float padding = 28.f;
        const float line_step = static_cast<float>(small_size) * 1.6f;

        DrawCenteredText("Bot Championship",
                         title_size,
                         {static_cast<float>(window_size.x) / 2.f,
                          panel_origin.y - 58.f},
                         {255, 255, 255},
                         true);

        DrawPanel(panel_origin, {panel_width, panel_height}, 18.f, 2.f);

        DrawTopLeftText("Rounds: " + std::to_string(stats.rounds_)
                        + "  Draws: " + std::to_string(stats.draws_)
                        + "  Timeouts: " + std::to_string(stats.timeouts_),
                        body_size,
                        {panel_origin.x + padding, panel_origin.y + padding},
                        {255, 255, 255},
                        true);

        for(std::size_t i = 0; i < stats.entries_.size(); ++i) {
            const auto& entry = stats.entries_[i];
            const float row_top = panel_origin.y + padding + 56.f + static_cast<float>(i) * 110.f;

            DrawStatsPreview({panel_origin.x + padding, row_top + 6.f},
                             18.f,
                             GetSnakeColor(entry.color_));
            DrawTopLeftText(entry.label_,
                            body_size,
                            {panel_origin.x + padding + 72.f, row_top},
                            {255, 255, 255},
                            true);
            DrawTopLeftText("Wins: " + std::to_string(entry.wins_)
                            + "  Survived: " + std::to_string(entry.survived_rounds_),
                            small_size,
                            {panel_origin.x + padding + 72.f, row_top + line_step},
                            {255, 255, 255});
            DrawTopLeftText("Kills: " + std::to_string(entry.total_kills_),
                            small_size,
                            {panel_origin.x + padding + 72.f, row_top + 2.f * line_step},
                            {255, 255, 255});
        }
    }

    [[nodiscard]] std::optional<Event> TranslateEvent(const sf::Event& event)
    {
        if(event.is<sf::Event::Closed>()) {
            window_.close();
            return Event::KEY_PRESSED_EXIT;
        }

        if(const auto* resized = event.getIf<sf::Event::Resized>()) {
            UpdateView(resized->size);
            return Event::RESIZE_WINDOW;
        }

        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if(key == nullptr)
            return std::nullopt;

        switch(key->code) {
            case sf::Keyboard::Key::W:      return Event::KEY_PRESSED_P1_UP;
            case sf::Keyboard::Key::S:      return Event::KEY_PRESSED_P1_DOWN;
            case sf::Keyboard::Key::A:      return Event::KEY_PRESSED_P1_LEFT;
            case sf::Keyboard::Key::D:      return Event::KEY_PRESSED_P1_RIGHT;
            case sf::Keyboard::Key::Up:     return Event::KEY_PRESSED_P2_UP;
            case sf::Keyboard::Key::Down:   return Event::KEY_PRESSED_P2_DOWN;
            case sf::Keyboard::Key::Left:   return Event::KEY_PRESSED_P2_LEFT;
            case sf::Keyboard::Key::Right:  return Event::KEY_PRESSED_P2_RIGHT;
            case sf::Keyboard::Key::P:      return Event::KEY_PRESSED_PAUSE;
            case sf::Keyboard::Key::Escape:
            case sf::Keyboard::Key::Q:
                window_.close();
                return Event::KEY_PRESSED_EXIT;

            default: return std::nullopt;
        }
    }
};

GraphicalView::GraphicalView() : impl_(std::make_unique<Impl>()) {}

GraphicalView::~GraphicalView() = default;

std::optional<Event> GraphicalView::PollEvents()
{
    if(!impl_->is_window_created_ || !impl_->window_.isOpen())
        return std::nullopt;

    while(const auto event = impl_->window_.pollEvent())
        if(const auto translated = impl_->TranslateEvent(*event))
            return translated;

    return std::nullopt;
}

bool GraphicalView::CanRenderGameplay(const Model& model) const
{
    (void) model;
    return !impl_->is_window_created_ || impl_->window_.isOpen();
}

void GraphicalView::Render(Model& model)
{
    impl_->EnsureWindow(model);
    if(!impl_->window_.isOpen())
        return;

    impl_->SyncSnakeStats(model);
    const Layout layout = impl_->GetLayout(model);

    impl_->window_.clear({8, 10, 16});
    impl_->DrawBackground();
    impl_->DrawTitle(layout);
    impl_->DrawPanel(layout.field_panel_origin_,
                     layout.field_panel_size_,
                     layout.panel_radius_,
                     layout.outline_thickness_);

    for(const auto& rabbit : model.rabbits_)
        impl_->DrawRabbit(rabbit, layout);

    for(const auto& snake : model.snakes_)
        impl_->DrawSnake(snake, layout);

    impl_->DrawStatsWindow(layout);
    impl_->window_.display();
}

void GraphicalView::RenderBotChampionship(const BotChampionshipStats& stats)
{
    impl_->EnsureSummaryWindow();
    if(!impl_->window_.isOpen())
        return;

    impl_->window_.clear({8, 10, 16});
    impl_->DrawBackground();
    impl_->DrawBotChampionship(stats);
    impl_->window_.display();
}

} // namespace snake_game

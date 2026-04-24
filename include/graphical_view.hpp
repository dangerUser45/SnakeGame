#pragma once

#include <memory>
#include <optional>
#include <string>

#include <SFML/System/Vector2.hpp>

#include "view.hpp"

namespace snake_game {

class GraphicalView final : public View {
public:
    GraphicalView();
    ~GraphicalView() override;

    [[nodiscard]] std::optional<Event> PollEvents() override;
    [[nodiscard]] bool CanRenderGameplay(const Model& model) const override;
    void Render(Model& model) override;

private:
    struct SnakeStatsEntry final {
        ObjColor color_ = ObjColor::WITHOUT_COLOR;
        std::string controller_label_{};
        int score_ = 0;
        int kills_ = 0;
        bool is_live_ = true;
    };

    struct Layout final {
        Coord inner_field_size_{1, 1};
        unsigned int title_char_size_ = 48;
        unsigned int body_char_size_ = 18;
        unsigned int small_char_size_ = 14;
        float cell_size_ = 16.f;
        float field_padding_ = 4.f;
        float stats_padding_ = 18.f;
        float panel_radius_ = 16.f;
        float outline_thickness_ = 2.f;
        float preview_cell_ = 16.f;
        float stats_width_ = 280.f;
        float title_height_ = 80.f;
        float entry_height_ = 110.f;
        float body_width_ = 0.f;
        float body_height_ = 0.f;
        sf::Vector2f title_center_{};
        sf::Vector2f body_origin_{};
        sf::Vector2f field_panel_origin_{};
        sf::Vector2f field_panel_size_{};
        sf::Vector2f field_inner_origin_{};
        sf::Vector2f stats_panel_origin_{};
        sf::Vector2f stats_panel_size_{};
    };

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace snake_game

#pragma once

#include <optional>

#include "bot_championship.hpp"
#include "model.hpp"
#include "event.hpp"

namespace snake_game {

class View {
public:
    [[nodiscard]] virtual std::optional<Event> PollEvents() = 0;
    [[nodiscard]] virtual bool CanRenderGameplay(const Model&) const = 0;
    virtual void Render(Model&) = 0;
    virtual void RenderBotChampionship(const BotChampionshipStats&) = 0;
    virtual ~View() = default;
};

} // namespace snake_game

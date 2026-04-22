#include "view.hpp"

namespace snake_game {

class GraphicalView final : public View {
public:
    // TODO: сделать ф-цию для graphview
    std::optional<Event> PollEvents() override
    {
        return std::nullopt;
    }

    void Render( Model& model) override
    {
        //TODO убрать заглушку
        (void) model;
    }

private:

};

} // namespace snake_game

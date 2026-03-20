#include "view.hpp"
#include "controller.hpp"
#include "model.hpp"

int main()
{
    using namespace snake_game;

    Model model{{70, 30}};
    TextView txt_view{};

    Controller contoller{model, txt_view};
    contoller.Run();
}

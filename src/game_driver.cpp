#include "view.hpp"
#include "controller.hpp"

int main()
{
    namespace sg = snake_game;

    // if(view == graph)
    sg::Controller<sg::GraphicalView> g_controller;
    g_controller.Run();

    // if(view == text)
    snake_game::Controller<sg::TextView> t_controller;
    t_controller.Run();
}

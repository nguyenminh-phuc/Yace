#include <memory>
#include "Yace/yace.hpp"
#include "player.hpp"
#include "tic_tac.hpp"

namespace
{
    void update(ye::chip8 const& chip8);

    std::unique_ptr<tic_tac> tic_Tac;
}

int main()
{
    tic_Tac.reset(new tic_tac());
    tic_Tac->start();

    YACE_APPLICATION.initialize(
            YACE_SCREEN_WIDTH * 10,
            YACE_SCREEN_HEIGHT * 10,
            "TicTacToe - Yace Application",
            YACE_FRAMERATE * 100);
    YACE_APPLICATION.run("resources/TICTAC", std::bind(&update, std::placeholders::_1));
    YACE_APPLICATION.terminate();

    return 0;
}

namespace
{
    void update(ye::chip8 const& chip8)
    {
        tic_Tac->play(chip8);
    }
}

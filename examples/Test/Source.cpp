#include "Yace/yace.hpp"

void update(ye::chip8 const&)
{
}

int main()
{
    const uint32_t width = YACE_SCREEN_WIDTH * 10;
    const uint32_t height = YACE_SCREEN_HEIGHT * 10;
    const uint32_t framerate = YACE_FRAMERATE * 100;
    const std::string title = "TICTAC - Yace Application";
    const std::string file_path = "resources/TICTAC";

    YACE_APPLICATION.initialize(width, height, title, framerate);
    YACE_APPLICATION.run(file_path, std::bind(&update, std::placeholders::_1));
    YACE_APPLICATION.terminate();

    return 0;
}

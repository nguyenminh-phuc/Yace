#ifndef YACE_APPLICATION_HPP
#define YACE_APPLICATION_HPP

#include <functional>
#include <memory>
#include <string>
#include "Yace/config.hpp"
#include "Yace/non_copyable.hpp"

namespace ye
{
    class chip8;
    class graphics;
    class window;

    class YACE_API application : public non_copyable
    {
    public:
        static application& get_instance();

        void initialize(
            uint32_t width = YACE_SCREEN_WIDTH,
            uint32_t height = YACE_SCREEN_HEIGHT,
            std::string const& title = "Yace Application",
            uint32_t framerate = YACE_FRAMERATE);

        void run(std::string const& file_path, std::function<void(chip8 const& chip8)> const& update) const;

        void terminate();

        window const& get_window() const;

    private:
        application();

        ~application();

        bool glfw_initialized_;

        void render() const;

        void play_beep() const;

        uint32_t framerate_;

        std::unique_ptr<chip8> chip8_;

        std::unique_ptr<graphics> graphics_;

        std::unique_ptr<window> window_;
    };
}

#endif

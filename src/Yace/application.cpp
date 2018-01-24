#include "Yace/application.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <ios>
#include <iterator>
#include <map>
#include <utility>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Yace/chip8.hpp"
#include "Yace/graphics.hpp"
#include "Yace/keyboard.hpp"
#include "Yace/window.hpp"

namespace priv
{
    std::vector<uint8_t> create_bitmap(std::array<uint8_t, 64 * 32> const& graphics, uint32_t width, uint32_t height);

    std::vector<uint8_t> load_resource(std::string const& file_path);

    std::map<uint8_t, ye::key> const chip8_key_layout =
    {
        {static_cast<uint8_t>(0x1), ye::key::one},
        {static_cast<uint8_t>(0x2), ye::key::two},
        {static_cast<uint8_t>(0x3), ye::key::three},
        {static_cast<uint8_t>(0xC), ye::key::four},
        {static_cast<uint8_t>(0x4), ye::key::q},
        {static_cast<uint8_t>(0x5), ye::key::w},
        {static_cast<uint8_t>(0x6), ye::key::e},
        {static_cast<uint8_t>(0xD), ye::key::r},
        {static_cast<uint8_t>(0x7), ye::key::a},
        {static_cast<uint8_t>(0x8), ye::key::s},
        {static_cast<uint8_t>(0x9), ye::key::d},
        {static_cast<uint8_t>(0xE), ye::key::f},
        {static_cast<uint8_t>(0xA), ye::key::z},
        {static_cast<uint8_t>(0x0), ye::key::x},
        {static_cast<uint8_t>(0xB), ye::key::c},
        {static_cast<uint8_t>(0xF), ye::key::v}
    };
}

namespace ye
{
    application& application::get_instance()
    {
        static application instance;

        return instance;
    }

    void application::initialize(
        uint32_t const width,
        uint32_t const height,
        std::string const& title,
        uint32_t const framerate)
    {
        try
        {
            if (glfw_initialized_)
                throw std::runtime_error("GLFW: Failed to reinitialize GLFW.");

            if (glfwInit() != GL_TRUE)
                throw std::runtime_error("GLFW: Failed to initialize GLFW.");
            glfw_initialized_ = true;

            window_.reset(new window(width, height, title));
            glfwMakeContextCurrent(&window_->get_glfw_window());

            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK)
                throw std::runtime_error("GLEW: Failed to initialize GLEW.");

            const auto error = glGetError();
            // Ignore GL_INVALID_ENUM after glewInit(): https://stackoverflow.com/a/20035078
            if (error != GL_NO_ERROR && error != GL_INVALID_ENUM)
                throw std::runtime_error("GLEW: Failed to handle the GL_INVALID_ENUM error after initializing GLEW.");

            glViewport(0, 0, width, height);

            framerate_ = framerate;
            graphics_.reset(new graphics(chip8::width, chip8::height));
            chip8_.reset(new chip8());

            YACE_LOG("OpenGL: %s, GLSL: %s\n",
                reinterpret_cast<const char*>(glGetString(GL_VERSION)),
                reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)))
                
            ;
        }
        catch (std::exception const& e)
        {
            (void)e;
            YACE_LOG("%s\n", e.what());
            throw;
        }
        catch (...)
        {
            YACE_LOG("Unexpected error.\n");
            throw;
        }
    }

    void application::run(std::string const& file_path, std::function<void(chip8 const& chip8)> const& update) const
    {
        try
        {
            chip8_->load(priv::load_resource(file_path));

            while (!glfwWindowShouldClose(&window_->get_glfw_window()))
            {
                const auto start_time = std::chrono::system_clock::now();

                glfwPollEvents();

                chip8_->emulate_cycle();

                for (auto const& key : priv::chip8_key_layout)
                    chip8_->keys[key.first] = window_->get_keyboard().is_key_pressed(key.second) ? 1 : 0;

                render();

                play_beep();

                update(*chip8_);

                glfwSwapBuffers(&window_->get_glfw_window());

                if (glGetError() != GL_NO_ERROR)
                    throw std::runtime_error("OpenGL: Failed to handle an unknown OpenGL error.");

                const auto end_time = std::chrono::system_clock::now();
                const auto frame_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                const auto sleep_time = 1000.0 / framerate_ - frame_time;

                if (sleep_time > 0)
                {
                    const auto sleep_until_time = end_time + std::chrono::duration<double, std::milli>(sleep_time);
                    while (true)
                        if (std::chrono::system_clock::now() >= sleep_until_time)
                            break;
                }
            }
        }
        catch (std::exception const& e)
        {
            (void)e;
            YACE_LOG("%s\n", e.what());
            throw;
        }
        catch (...)
        {
            YACE_LOG("Unexpected error.\n");
            throw;
        }
    }

    void application::terminate()
    {
        if (glfw_initialized_)
        {
            glfw_initialized_ = false;
            glfwTerminate();
        }
    }

    window const& application::get_window() const
    {
        return *window_;
    }

    application::application() :
        glfw_initialized_(false),
        framerate_(0)
    {
    }

    application::~application()
    {
        terminate();
    }

    void application::render() const
    {
        if (chip8_->redraw_flag)
        {
            chip8_->redraw_flag = false;
            graphics_->set_bitmap(
                priv::create_bitmap(chip8_->graphics, chip8::width, chip8::height),
                chip8::width,
                chip8::height);
        }

        graphics_->render();
    }

    void application::play_beep() const
    {
        if (chip8_->sound_flag)
        {
            chip8_->sound_flag = false;

            // TODO: Use PCM to generate a beep sound.
            YACE_LOG("BEEP!\n");
        }
    }
}

namespace priv
{
    std::vector<uint8_t> create_bitmap(
        std::array<uint8_t, 64 * 32> const& graphics,
        uint32_t const width,
        uint32_t const height)
    {
        std::vector<uint8_t> bitmap(width * height * 3);

        for (uint32_t y = 0; y < height; ++y)
        {
            for (uint32_t x = 0; x < width; ++x)
            {
                if (graphics[y * width + x] == 0)
                {
                    bitmap[y * width * 3 + x * 3 + 0] = bitmap[y * width * 3 + x * 3 + 1] = bitmap[y * width * 3 + x * 3
                        + 2] = 0;
                }
                else
                {
                    bitmap[y * width * 3 + x * 3 + 0] = bitmap[y * width * 3 + x * 3 + 1] = bitmap[y * width * 3 + x * 3
                        + 2] = 255;
                }
            }
        }

        return bitmap;
    }

    std::vector<uint8_t> load_resource(std::string const& file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Application: Failed to load resource file.");

        file.unsetf(std::ios::skipws);

        std::vector<uint8_t> buffer;
        buffer.insert(buffer.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());

        return buffer;
    }
}

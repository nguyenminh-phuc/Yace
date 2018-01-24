#ifndef YACE_WINDOW_HPP
#define YACE_WINDOW_HPP

#include <memory>
#include <string>
#include "Yace/config.hpp"
#include "Yace/non_copyable.hpp"

struct GLFWwindow;

namespace ye
{
    class keyboard;

    class YACE_API window : public non_copyable
    {
    public:
        window() = delete;

        window(uint32_t width, uint32_t height, std::string const& title);

        GLFWwindow& get_glfw_window() const;

        keyboard& get_keyboard() const;

        void set_title(std::string const& title) const;

        void resize(uint32_t width, uint32_t height) const;

    private:
        std::unique_ptr<keyboard> keyboard_;

        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> glfw_window_;
    };
}

#endif

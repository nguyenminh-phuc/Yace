#include "Yace/window.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Yace/keyboard.hpp"

namespace priv
{
    void destroy(GLFWwindow* glfw_window);

    void window_size_callback(GLFWwindow* glfw_window, int width, int height);
}

namespace ye
{
    window::window(uint32_t const width, uint32_t const height, std::string const& title) :
        glfw_window_(nullptr, priv::destroy)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YACE_OPENGL_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YACE_OPENGL_MINOR);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfw_window_.reset(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));
        if (!glfw_window_.get())
            throw std::runtime_error("GLFW: Failed to create window.");

        keyboard_.reset(new keyboard(*this));

        glfwSetWindowUserPointer(glfw_window_.get(), this);
        glfwSetWindowSizeCallback(glfw_window_.get(), priv::window_size_callback);
    }

    GLFWwindow& window::get_glfw_window() const
    {
        return *glfw_window_;
    }

    keyboard& window::get_keyboard() const
    {
        return *keyboard_;
    }

    void window::set_title(std::string const& title) const
    {
        glfwSetWindowTitle(glfw_window_.get(), title.c_str());
    }

    void window::resize(uint32_t const width, uint32_t const height) const
    {
        glViewport(0, 0, width, height);
    }
}

namespace priv
{
    void destroy(GLFWwindow* glfw_window)
    {
        glfwDestroyWindow(glfw_window);
    }

    void window_size_callback(GLFWwindow* glfw_window, const int width, const int height)
    {
        static_cast<ye::window*>(glfwGetWindowUserPointer(glfw_window))->resize(width, height);
    }
}

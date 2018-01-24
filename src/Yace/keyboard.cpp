#include "Yace/keyboard.hpp"

#include <map>
#include "GLFW/glfw3.h"
#include "Yace/window.hpp"

namespace priv
{
    void key_callback(GLFWwindow* glfw_window, int glfw_key, int scancode, int action, int mods);

    std::map<int, ye::key> converted_keys =
    {
        {GLFW_KEY_1, ye::key::one},
        {GLFW_KEY_2, ye::key::two},
        {GLFW_KEY_3, ye::key::three},
        {GLFW_KEY_4, ye::key::four},
        {GLFW_KEY_Q, ye::key::q},
        {GLFW_KEY_W, ye::key::w},
        {GLFW_KEY_E, ye::key::e},
        {GLFW_KEY_R, ye::key::r},
        {GLFW_KEY_A, ye::key::a},
        {GLFW_KEY_S, ye::key::s},
        {GLFW_KEY_D, ye::key::d},
        {GLFW_KEY_F, ye::key::f},
        {GLFW_KEY_Z, ye::key::z},
        {GLFW_KEY_X, ye::key::x},
        {GLFW_KEY_C, ye::key::c},
        {GLFW_KEY_V, ye::key::v}
    };

    std::map<ye::key, char> keys_to_chars =
    {
        {ye::key::unknown, '?'},
        {ye::key::one, '1'},
        {ye::key::two, '2'},
        {ye::key::three, '3'},
        {ye::key::four, '4'},
        {ye::key::f, 'f'},
        {ye::key::q, 'q'},
        {ye::key::w, 'w'},
        {ye::key::e, 'e'},
        {ye::key::r, 'r'},
        {ye::key::a, 'a'},
        {ye::key::s, 's'},
        {ye::key::d, 'd'},
        {ye::key::f, 'f'},
        {ye::key::z, 'z'},
        {ye::key::x, 'x'},
        {ye::key::c, 'c'},
        {ye::key::v, 'v'}
    };

    ye::keyboard* keyboard = nullptr;
}

namespace ye
{
    keyboard::keyboard(window const& window)
    {
        key_states_[key::unknown] =
            key_states_[key::one] = key_states_[key::two] = key_states_[key::three] = key_states_[key::four] =
            key_states_[key::q] = key_states_[key::w] = key_states_[key::e] = key_states_[key::r] =
            key_states_[key::a] = key_states_[key::s] = key_states_[key::d] = key_states_[key::f] =
            key_states_[key::z] = key_states_[key::x] = key_states_[key::c] = key_states_[key::v] = false;

        priv::keyboard = this;
        glfwSetKeyCallback(&window.get_glfw_window(), priv::key_callback);
    }

    keyboard::~keyboard()
    {
        priv::keyboard = nullptr;
    }

    void keyboard::press(key const key)
    {
        YACE_LOG("Key %c is pressed.\n", priv::keys_to_chars[key]);
        key_states_[key] = true;
    }

    void keyboard::release(key const key)
    {
        YACE_LOG("Key %c is released.\n", priv::keys_to_chars[key]);
        key_states_[key] = false;
    }

    bool keyboard::is_key_pressed(key const key) const
    {
        return key_states_.find(key)->second;
    }
}

namespace priv
{
    void key_callback(GLFWwindow*, const int glfw_key, int, const int action, int)
    {
        const auto it = converted_keys.find(glfw_key);
        const auto key = it != converted_keys.end() ? it->second : ye::key::unknown;

        if (action == GLFW_PRESS)
            keyboard->press(key);
        else if (action == GLFW_RELEASE)
            keyboard->release(key);
    }
}

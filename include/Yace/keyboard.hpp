#ifndef YACE_KEYBOARD_HPP
#define YACE_KEYBOARD_HPP

#include <map>
#include "Yace/config.hpp"
#include "Yace/non_copyable.hpp"

namespace ye
{
    class window;

    enum class key
    {
        unknown,
        one,
        two,
        three,
        four,
        q,
        w,
        e,
        r,
        a,
        s,
        d,
        f,
        z,
        x,
        c,
        v
    };

    class YACE_API keyboard : public non_copyable
    {
    public:
        keyboard() = delete;

        explicit keyboard(window const& window);

        ~keyboard();

        void press(key key);

        void release(key key);

        bool is_key_pressed(key key) const;

    private:
        std::map<key, bool> key_states_;
    };
}

#endif

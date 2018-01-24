#ifndef YACE_NON_COPYABLE_HPP
#define YACE_NON_COPYABLE_HPP

#include "Yace/config.hpp"

namespace ye
{
    class YACE_API non_copyable
    {
    public:
        non_copyable() = default;

        ~non_copyable() = default;

        non_copyable(non_copyable const&) = delete;

        non_copyable& operator =(non_copyable const&) = delete;

        non_copyable(non_copyable&&) = delete;

        non_copyable& operator=(non_copyable&&) = delete;
    };
}

#endif

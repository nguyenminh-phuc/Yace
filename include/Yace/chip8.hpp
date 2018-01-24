#ifndef YACE_CHIP8_HPP
#define YACE_CHIP8_HPP

#include <array>
#include <vector>
#include "Yace/config.hpp"
#include "Yace/non_copyable.hpp"

namespace ye
{
    class YACE_API chip8 : public non_copyable
    {
    public:
        static uint32_t const height;

        static uint32_t const width;

        chip8();

        void load(std::vector<uint8_t> const& buffer);

        void emulate_cycle();

        uint16_t get_opcode() const;

        bool redraw_flag;

        bool sound_flag;

        std::array<uint8_t, 64 * 32> graphics;

        std::array<uint8_t, 16> keys;

    private:
        uint16_t opcode_;

        std::array<uint8_t, 4096> memory_;

        std::array<uint8_t, 16> registers_;

        std::array<uint16_t, 16> stack_;

        uint16_t address_register_;

        uint16_t pc_;

        uint8_t delay_timer_;

        uint8_t sound_timer_;

        uint8_t stack_ptr_;
    };
}

#endif

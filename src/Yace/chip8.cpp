#include "Yace/chip8.hpp"

#include <random>

namespace priv
{
    uint8_t generate_random_number(uint8_t min, uint8_t max);

    std::mt19937 mersenne_twister{std::random_device()()};

    std::array<uint8_t, 16 * 5> const fontset = std::array<uint8_t, 80>
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };
}

namespace ye
{
    uint32_t const chip8::height = 32;

    uint32_t const chip8::width = 64;

    chip8::chip8() :
        redraw_flag(false),
        sound_flag(false),
        graphics({0}),
        keys({0}),
        opcode_(0),
        memory_({0}),
        registers_({0}),
        stack_({0}),
        address_register_(0),
        pc_(0),
        delay_timer_(0),
        sound_timer_(0),
        stack_ptr_(0)
    {
    }

    void chip8::load(std::vector<uint8_t> const& buffer)
    {
        redraw_flag = true;
        sound_flag = false;
        graphics.fill(0);
        keys.fill(0);
        opcode_ = 0;
        memory_.fill(0);
        registers_.fill(0);
        address_register_ = 0;
        pc_ = 0x200; // program memory location starts at 0x200
        delay_timer_ = 0;
        sound_timer_ = 0;
        stack_.fill(0);
        stack_ptr_ = 0;

        std::copy(priv::fontset.begin(), priv::fontset.end(), memory_.begin());
        std::copy(buffer.begin(), buffer.end(), memory_.begin() + 0x200); // program memory location starts at 0x200
    }

    void chip8::emulate_cycle()
    {
        // Fetch opcode
        opcode_ = memory_[pc_] << 8 | memory_[pc_ + 1];

        //YACE_LOG("\t%x\n", opcode_);

        // Decode & execute opcode
        // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
        // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
        // x - A 4-bit value, the lower 4 bits of the high uint8_t of the instruction
        // y - A 4-bit value, the upper 4 bits of the low uint8_t of the instruction
        // kk or uint8_t - An 8-bit value, the lowest 8 bits of the instruction

        // 00E0 - CLS
        // Clear the display.
        if (opcode_ == 0x00E0)
        {
            graphics.fill(0);
            redraw_flag = true;
            pc_ += 2;
        }

            // 00EE - RET
            // Return from a subroutine.
        else if (opcode_ == 0x00EE)
        {
            --stack_ptr_;
            pc_ = stack_[stack_ptr_];
            pc_ += 2;
        }

            // 1nnn - JP addr
            // Jump to location nnn.
        else if ((opcode_ & 0xF000) == 0x1000)
            pc_ = opcode_ & 0x0FFF;

            // 2nnn - CALL addr
            // Calls subroutine at nnn.
        else if ((opcode_ & 0xF000) == 0x2000)
        {
            stack_[stack_ptr_] = pc_;
            ++stack_ptr_;
            pc_ = opcode_ & 0x0FFF;
        }

            // 3xkk - SE Vx, uint8_t
            // Skip next instruction if Vx = kk.
        else if ((opcode_ & 0xF000) == 0x3000)
            if (registers_[(opcode_ & 0x0F00) >> 8] == (opcode_ & 0x00FF))
                pc_ += 4;
            else
                pc_ += 2;

            // 4xkk - SNE Vx, uint8_t
            // Skip next instruction if Vx != kk.
        else if ((opcode_ & 0xF000) == 0x4000)
            if (registers_[(opcode_ & 0x0F00) >> 8] != (opcode_ & 0x00FF))
                pc_ += 4;
            else
                pc_ += 2;

            //  5xy0 - SE Vx, Vy
            // Skip next instruction if Vx = Vy.
        else if ((opcode_ & 0xF00F) == 0x5000)
            if (registers_[(opcode_ & 0x0F00) >> 8] == registers_[(opcode_ & 0x00F0) >> 4])
                pc_ += 4;
            else
                pc_ += 2;

            // 6xkk - LD Vx, uint8_t
            // Set Vx = kk.
        else if ((opcode_ & 0xF000) == 0x6000)
        {
            registers_[(opcode_ & 0x0F00) >> 8] = opcode_ & 0x00FF;
            pc_ += 2;
        }

            // 7xkk - ADD Vx, uint8_t
            // Set Vx = Vx + kk.
        else if ((opcode_ & 0xF000) == 0x7000)
        {
            registers_[(opcode_ & 0x0F00) >> 8] += opcode_ & 0x00FF;
            pc_ += 2;
        }

            // 8xy0 - LD Vx, Vy
            // Set Vx = Vy.
        else if ((opcode_ & 0xF00F) == 0x8000)
        {
            registers_[(opcode_ & 0x0F00) >> 8] = registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy1 - OR Vx, Vy
            // Set Vx = Vx OR Vy.
        else if ((opcode_ & 0xF00F) == 0x8001)
        {
            registers_[(opcode_ & 0x0F00) >> 8] |= registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy2 - AND Vx, Vy
            // Set Vx = Vx AND Vy.
        else if ((opcode_ & 0xF00F) == 0x8002)
        {
            registers_[(opcode_ & 0x0F00) >> 8] &= registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy3 - XOR Vx, Vy
            // Set Vx = Vx XOR Vy.
        else if ((opcode_ & 0xF00F) == 0x8003)
        {
            registers_[(opcode_ & 0x0F00) >> 8] ^= registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy4 - ADD Vx, Vy
            // Set Vx = Vx + Vy, set VF = carry.
        else if ((opcode_ & 0xF00F) == 0x8004)
        {
            if (registers_[(opcode_ & 0x0F00) >> 8] + registers_[(opcode_ & 0x00F0) >> 4] > 0xFF)
                registers_[0xF] = 1;
            else
                registers_[0xF] = 0;
            registers_[(opcode_ & 0x0F00) >> 8] += registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy5 - SUB Vx, Vy
            // Set Vx = Vx - Vy, set VF = NOT borrow.
        else if ((opcode_ & 0xF00F) == 0x8005)
        {
            if (registers_[(opcode_ & 0x0F00) >> 8] > registers_[(opcode_ & 0x00F0) >> 4])
                registers_[0xF] = 1;
            else
                registers_[0xF] = 0;
            registers_[(opcode_ & 0x0F00) >> 8] -= registers_[(opcode_ & 0x00F0) >> 4];
            pc_ += 2;
        }

            // 8xy6 - SHR Vx {, Vy}
            // Set Vx = Vx SHR 1.
        else if ((opcode_ & 0xF00F) == 0x8006)
        {
            registers_[0xF] = registers_[(opcode_ & 0x0F00) >> 8] & 0x01;
            registers_[(opcode_ & 0x0F00) >> 8] >>= 1;
            pc_ += 2;
        }

            // 8xy7 - SUBN Vx, Vy
            // Set Vx = Vy - Vx, set VF = NOT borrow.
        else if ((opcode_ & 0xF00F) == 0x8007)
        {
            if (registers_[(opcode_ & 0x00F0) >> 4] > registers_[(opcode_ & 0x0F00) >> 8])
                registers_[0xF] = 1;
            else
                registers_[0xF] = 0;
            registers_[(opcode_ & 0x0F00) >> 8] = registers_[(opcode_ & 0x00F0) >> 4] - registers_[(opcode_ & 0x0F00) >>
                8];
            pc_ += 2;
        }

            // 8xyE - SHL Vx {, Vy}
            // Set Vx = Vx SHL 1.
        else if ((opcode_ & 0xF00F) == 0x800E)
        {
            registers_[0xF] = registers_[(opcode_ & 0x0F00) >> 8] >> 7;
            registers_[(opcode_ & 0x0F00) >> 8] <<= 1;
            pc_ += 2;
        }

            // 9xy0 - SNE Vx, Vy
            // Skip next instruction if Vx != Vy.
        else if ((opcode_ & 0xF00F) == 0x9000)
        {
            if (registers_[(opcode_ & 0x0F00) >> 8] != registers_[(opcode_ & 0x00F0) >> 4])
                pc_ += 4;
            else
                pc_ += 2;
        }

            // Annn - LD I, addr
            // Set I = nnn.
        else if ((opcode_ & 0xF000) == 0xA000)
        {
            address_register_ = opcode_ & 0x0FFF;
            pc_ += 2;
        }

            // Bnnn - JP V0, addr
            // Jump to location nnn + V0.
        else if ((opcode_ & 0xF000) == 0xB000)
            pc_ = (opcode_ & 0x0FFF) + registers_[0x0];

            // Cxkk - RND Vx, uint8_t
            // Set Vx = random uint8_t AND kk.
        else if ((opcode_ & 0xF000) == 0xC000)
        {
            registers_[(opcode_ & 0x0F00) >> 8] = priv::generate_random_number(0x00, 0xFF) & (opcode_ & 0x00FF);
            pc_ += 2;
        }

            // Dxyn - DRW Vx, Vy, nibble
            // Display n - uint8_t sprite starting at memory location I at(Vx, Vy), set VF = collision.
        else if ((opcode_ & 0xF000) == 0xD000)
        {
            const uint16_t x = registers_[(opcode_ & 0x0F00) >> 8];
            const uint16_t y = registers_[(opcode_ & 0x00F0) >> 4];
            const uint32_t x_width = 8;
            const uint32_t y_height = opcode_ & 0x000F;
            registers_[0xF] = 0;
            for (uint32_t y_line = 0; y_line < y_height; ++y_line)
            {
                const uint16_t pixel = memory_[address_register_ + y_line];
                for (uint32_t x_line = 0; x_line < x_width; ++x_line)
                    // Check if the current evaluated pixel is set to 1
                    if ((pixel & (0x80 >> x_line)) != 0)
                        // Prevent "array subscript out of range" error
                        if (x + x_line + ((y + y_line) * width) < graphics.size())
                        {
                            // Check if the pixel on the display is set to 1
                            if (graphics[x + x_line + ((y + y_line) * width)] == 1)
                                registers_[0xF] = 1;
                            graphics[x + x_line + ((y + y_line) * width)] ^= 1;
                        }
            }
            redraw_flag = true;
            pc_ += 2;
        }

            // Ex9E - SKP Vx
            // Skip next instruction if key with the value of Vx is pressed.
        else if ((opcode_ & 0xF0FF) == 0xE09E)
            if (keys[registers_[(opcode_ & 0x0F00) >> 8]] != 0)
                pc_ += 4;
            else
                pc_ += 2;

            // ExA1 - SKNP Vx
            // Skip next instruction if key with the value of Vx is not pressed.
        else if ((opcode_ & 0xF0FF) == 0xE0A1)
            if (keys[registers_[(opcode_ & 0x0F00) >> 8]] == 0)
                pc_ += 4;
            else
                pc_ += 2;

            // Fx07 - LD Vx, DT
            // Set Vx = delay timer value.
        else if ((opcode_ & 0xF0FF) == 0xF007)
        {
            registers_[(opcode_ & 0x0F00) >> 8] = delay_timer_;
            pc_ += 2;
        }

            // Fx0A - LD Vx, K
            // Wait for a key press, store the value of the key in Vx.
        else if ((opcode_ & 0xF0FF) == 0xF00A)
        {
            auto key_press = false;
            for (size_t i = 0; i < keys.size(); ++i)
            {
                if (keys[i] != 0)
                {
                    registers_[(opcode_ & 0x0F00) >> 8] = static_cast<uint8_t>(i);
                    key_press = true;
                }
            }
            if (!key_press)
                return;
            pc_ += 2;
        }

            // Fx15 - LD DT, Vx
            // Set delay timer = Vx.
        else if ((opcode_ & 0xF0FF) == 0xF015)
        {
            delay_timer_ = registers_[(opcode_ & 0x0F00) >> 8];
            pc_ += 2;
        }

            // Fx18 - LD ST, Vx
            // Set sound timer = Vx.
        else if ((opcode_ & 0xF0FF) == 0xF018)
        {
            sound_timer_ = registers_[(opcode_ & 0x0F00) >> 8];
            pc_ += 2;
        }

            // Fx1E - ADD I, Vx
            // Set I = I + Vx.
        else if ((opcode_ & 0xF0FF) == 0xF01E)
        {
            if (address_register_ + registers_[(opcode_ & 0x0F00) >> 8] > 0x0FFF)
                registers_[0xF] = 1;
            else
                registers_[0xF] = 0;
            address_register_ += registers_[(opcode_ & 0x0F00) >> 8];
            pc_ += 2;
        }

            // Fx29 - LD F, Vx
            //Set I = location of sprite for digit Vx.
        else if ((opcode_ & 0xF0FF) == 0xF029)
        {
            address_register_ = registers_[((opcode_ & 0x0F00) >> 8)] * 0x5;
            pc_ += 2;
        }

            // Fx33 - LD B, Vx
            // Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
        else if ((opcode_ & 0xF0FF) == 0xF033)
        {
            memory_[address_register_] = registers_[(opcode_ & 0x0F00) >> 8] / 100;
            memory_[address_register_ + 1] = (registers_[(opcode_ & 0x0F00) >> 8] / 10) % 10;
            memory_[address_register_ + 2] = (registers_[(opcode_ & 0x0F00) >> 8] % 100) % 10;
            pc_ += 2;
        }

            // Fx55 - LD [I], Vx
            // Store registers V0 through Vx in memory starting at location I.
        else if ((opcode_ & 0xF0FF) == 0xF055)
        {
            for (size_t i = 0x0; i <= ((opcode_ & 0x0F00) >> 8); ++i)
                memory_[address_register_ + i] = registers_[i];
            pc_ += 2;
        }

            // Fx65 - LD Vx, [I]
            // Read registers V0 through Vx from memory starting at location I.
        else if ((opcode_ & 0xF0FF) == 0xF065)
        {
            for (size_t i = 0x0; i <= ((opcode_ & 0x0F00) >> 8); ++i)
                registers_[i] = memory_[address_register_ + i];
            pc_ += 2;
        }

        else
            throw std::runtime_error("Chip8: Failed to decode opcode.");

        if (delay_timer_ > 0)
            --delay_timer_;

        if (sound_timer_ > 0)
        {
            if (sound_timer_ == 1)
                sound_flag = true;

            --sound_timer_;
        }
    }

    uint16_t chip8::get_opcode() const
    {
        return opcode_;
    }
}

namespace priv
{
    uint8_t generate_random_number(uint8_t const min, uint8_t const max)
    {
        const std::uniform_int_distribution<uint32_t> distribution(min, max);

        return static_cast<uint8_t>(distribution(mersenne_twister));
    }
}

#ifndef YACE_GRAPHICS_HPP
#define YACE_GRAPHICS_HPP

#include <memory>
#include <vector>
#include <string>
#include "Yace/config.hpp"
#include "Yace/non_copyable.hpp"

namespace ye
{
    class YACE_API graphics : public non_copyable
    {
    public:
        graphics() = delete;

        graphics(uint32_t width, uint32_t height);

        ~graphics();

        void render() const;

        uint32_t get_height() const;

        uint32_t get_width() const;

        void set_bitmap(std::vector<uint8_t> const& bitmap, uint32_t width, uint32_t height);

    private:
        void create_vertex_input();

        void create_chip8_texture(uint32_t width, uint32_t height);

        void create_chip8_shader(std::string const& vertex, std::string const& fragment);

        uint32_t height_;

        uint32_t width_;

        uint32_t vao_id_;

        uint32_t vbo_id_;

        uint32_t ebo_id_;

        uint32_t texture_id_;

        uint32_t vertex_id_;

        uint32_t fragment_id_;

        uint32_t program_id_;

        std::vector<uint8_t> bitmap_;
    };
}

#endif

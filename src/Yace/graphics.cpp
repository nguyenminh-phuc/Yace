#include "Yace/graphics.hpp"

#include <map>
#include "GL/glew.h"

namespace priv
{
    void create_shader(
        std::string const& vertex,
        std::string const& fragment,
        uint32_t& vertext_id,
        uint32_t& fragment_id,
        uint32_t& program_id);

    std::string const vertex_source =
        "#version 330 core\n"
        "in vec3 position;"
        "in vec2 texCoord;"
        "out vec2 vsTexCoord;"
        "void main() {"
        "gl_Position = vec4(position, 1.0);"
        "vsTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);"
        "}";

    std::string const fragment_source =
        "#version 330 core\n"
        "in vec2 vsTexCoord;"
        "uniform sampler2D graphicsTexture;"
        "out vec4 color;"
        "void main() {"
        "color = texture(graphicsTexture, vsTexCoord);"
        "}";

    GLfloat const texture_vertices[] =
    {
        // Positions      // Texture Coords
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top Right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Top Left 
    };

    GLuint const texture_indices[] =
    {
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };
}

namespace ye
{
    graphics::graphics(uint32_t const width, uint32_t const height) :
        height_(0),
        width_(0),
        vao_id_(0),
        vbo_id_(0),
        ebo_id_(0),
        texture_id_(0),
        vertex_id_(0),
        fragment_id_(0),
        program_id_(0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        create_chip8_shader(priv::vertex_source, priv::fragment_source);
        create_chip8_texture(width, height);
        create_vertex_input();
    }

    graphics::~graphics()
    {
        glDetachShader(program_id_, vertex_id_);
        glDetachShader(program_id_, fragment_id_);
        glDeleteProgram(program_id_);
        glDeleteShader(vertex_id_);
        glDeleteShader(fragment_id_);
        glDeleteTextures(1, &texture_id_);
        glDeleteVertexArrays(1, &vao_id_);
        glDeleteBuffers(1, &vbo_id_);
        glDeleteBuffers(1, &ebo_id_);
    }

    void graphics::render() const
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program_id_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glUniform1i(glGetUniformLocation(program_id_, "graphicsTexture"), 0);

        glBindVertexArray(vao_id_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    uint32_t graphics::get_height() const
    {
        return height_;
    }

    uint32_t graphics::get_width() const
    {
        return width_;
    }

    void graphics::set_bitmap(std::vector<uint8_t> const& bitmap, uint32_t const width, uint32_t const height)
    {
        bitmap_ = bitmap;
        width_ = width;
        height_ = height;

        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, bitmap_.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void graphics::create_vertex_input()
    {
        glGenVertexArrays(1, &vao_id_);
        glGenBuffers(1, &vbo_id_);
        glGenBuffers(1, &ebo_id_);

        glBindVertexArray(vao_id_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
        glBufferData(GL_ARRAY_BUFFER, sizeof priv::texture_vertices, priv::texture_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof priv::texture_indices, priv::texture_indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void graphics::create_chip8_texture(uint32_t const width, uint32_t const height)
    {
        width_ = width;
        height_ = height;
        bitmap_.resize(3 * height_ * width_, 0);

        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap_.data());

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void graphics::create_chip8_shader(std::string const& vertex, std::string const& fragment)
    {
        priv::create_shader(vertex, fragment, vertex_id_, fragment_id_, program_id_);
    }
}

namespace priv
{
    void create_shader(
        std::string const& vertex,
        std::string const& fragment,
        uint32_t& vertext_id,
        uint32_t& fragment_id,
        uint32_t& program_id)
    {
        auto success = 0;
        char const* c_str;

        const auto tmp_vertex_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(tmp_vertex_id, 1, &(c_str = vertex.c_str()), nullptr);
        glCompileShader(tmp_vertex_id);
        glGetShaderiv(tmp_vertex_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glDeleteShader(tmp_vertex_id);
            throw std::runtime_error("OpenGL: Failed to create vertex shader.");
        }

        const auto tmp_fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(tmp_fragment_id, 1, &(c_str = fragment.c_str()), nullptr);
        glCompileShader(tmp_fragment_id);
        glGetShaderiv(tmp_fragment_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glDeleteShader(tmp_vertex_id);
            glDeleteShader(tmp_fragment_id);
            throw std::runtime_error("OpenGL: Failed to create fragment shader.");
        }

        const auto tmp_program_id = glCreateProgram();
        glAttachShader(tmp_program_id, tmp_vertex_id);
        glAttachShader(tmp_program_id, tmp_fragment_id);
        glLinkProgram(tmp_program_id);
        glGetProgramiv(tmp_program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glDeleteShader(tmp_vertex_id);
            glDeleteShader(tmp_fragment_id);
            glDeleteProgram(tmp_program_id);
            throw std::runtime_error("OpenGL: Failed to create program.");
        }

        vertext_id = tmp_vertex_id;
        fragment_id = tmp_fragment_id;
        program_id = tmp_program_id;
    }
}

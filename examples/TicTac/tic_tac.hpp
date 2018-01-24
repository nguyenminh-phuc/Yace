#ifndef TICTAC_TICTAC_HPP
#define TICTAC_TICTAC_HPP

#include <array>
#include <memory>
#include "Yace/yace.hpp"
#include "player.hpp"

enum class cell_state
{
    empty,
    x,
    o
};

enum class game_state
{
    unknown,
    new_,
    draw,
    win,
};

class tic_tac : public ye::non_copyable
{
public:
    static std::array<ye::key, 9> tic_tac_keys;

    tic_tac();

    void start();

    void play(ye::chip8 const& chip8);

    bool try_mark(uint32_t position) const;

    game_state get_game_state() const;

    char get_current_player() const;

    std::array<cell_state, 9> get_board() const;

private:
    bool marked_;

    bool wait_clear_board_;

    std::unique_ptr<uint32_t> marking_position_;

    char current_player_;

    player player_x_;

    player player_o_;

    ye::chip8 const* chip8_;
};

#endif

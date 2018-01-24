#include "tic_tac.hpp"

#include <algorithm>
#include <array>
#include <tuple>
#include <sstream>
#include "player.hpp"

namespace priv
{
    bool is_pressed(ye::chip8 const& chip8, uint32_t position);

    std::array<std::tuple<uint32_t, uint32_t, uint32_t>, 8> winable_moves =
    {
        std::make_tuple(0, 1, 2),
        std::make_tuple(3, 4, 5),
        std::make_tuple(6, 7, 8),
        std::make_tuple(0, 3, 6),
        std::make_tuple(1, 4, 7),
        std::make_tuple(2, 5, 8),
        std::make_tuple(0, 4, 8),
        std::make_tuple(2, 4, 6)
    };

    std::array<uint32_t, 9> chip8_o_positions = {342, 350, 358, 854, 862, 870, 1366, 1374, 1382};

    std::array<uint32_t, 9> chip8_x_positions = {341, 349, 357, 853, 861, 869, 1365, 1373, 1381};
}

std::array<ye::key, 9> tic_tac::tic_tac_keys =
{
    ye::key::one,
    ye::key::two,
    ye::key::three,
    ye::key::q,
    ye::key::w,
    ye::key::e,
    ye::key::a,
    ye::key::s,
    ye::key::d,
};

tic_tac::tic_tac() :
    marked_(false),
    wait_clear_board_(false),
    current_player_(0),
    player_x_('X'),
    player_o_('O'),
    chip8_(nullptr)
{
}

void tic_tac::start()
{
    marked_ = false;
    marking_position_.reset();

    if (player_x_.name == player_o_.name)
        throw std::runtime_error("TicTac: Failed to start game with 2 players who have the same names.");

    current_player_ = player_o_.name;
    player_x_.start_game();
    player_o_.start_game();
}

void tic_tac::play(ye::chip8 const& chip8)
{
    chip8_ = &chip8;
    auto const& board = get_board();
    const game_state game_state = get_game_state();

    player* other_player;
    player* player;
    if (current_player_ == player_o_.name)
    {
        player = &player_o_;
        other_player = &player_x_;
    }
    else
    {
        player = &player_x_;
        other_player = &player_o_;
    }

    if (marked_)
    {
        if (game_state == game_state::win)
        {
            std::stringstream stream;
            stream << "TicTacToe - " << (current_player_ == player_o_.name ? player_x_.name : player_o_.name) << " won";
            YACE_WINDOW.set_title(stream.str());
            player->set_reward(1.0f, board);
            other_player->set_reward(-1.0f, board);

            wait_clear_board_ = true;
        }
        else if (game_state == game_state::draw) // tie game
        {
            YACE_WINDOW.set_title("TicTacToe - Draw");
            player->set_reward(0.5f, board);
            other_player->set_reward(0.5f, board);

            wait_clear_board_ = true;
        }
        else
            other_player->set_reward(0.0f, board);

        current_player_ = current_player_ == player_o_.name ? player_x_.name : player_o_.name;
        marked_ = false;
    }
    else
    {
        if (wait_clear_board_)
        {
            auto& keyboard = YACE_WINDOW.get_keyboard();

            if (game_state == game_state::new_)
            {
                if (keyboard.is_key_pressed(ye::key::z))
                    keyboard.release(ye::key::z);
                wait_clear_board_ = false;
            }
            else
            {
                keyboard.press(ye::key::z);

                return;
            }
        }

        if (!marking_position_.get())
            marking_position_.reset(new uint32_t(player->move(board)));

        if (try_mark(*marking_position_))
        {
            marked_ = true;
            marking_position_.reset();
        }
    }
}

game_state tic_tac::get_game_state() const
{
    auto const& board = get_board();

    auto new_game = true;
    for (auto const cell : board)
        if (cell == cell_state::x || cell == cell_state::o)
        {
            new_game = false;
            break;
        }
    if (new_game)
        return game_state::new_;

    const auto player_cell = current_player_ == player_o_.name ? cell_state::o : cell_state::x;
    for (auto const& move : priv::winable_moves)
        if (board[std::get<0>(move)] == player_cell &&
            board[std::get<1>(move)] == player_cell &&
            board[std::get<2>(move)] == player_cell)
            return game_state::win;

    if (std::find(board.begin(), board.end(), cell_state::empty) == board.end())
        return game_state::draw;

    return game_state::unknown;
}

bool tic_tac::try_mark(const uint32_t position) const
{
    if (!priv::is_pressed(*chip8_, position))
    {
        for (auto const default_key : tic_tac_keys)
            YACE_WINDOW.get_keyboard().release(default_key);
        YACE_WINDOW.get_keyboard().press(tic_tac_keys[position]);

        return false;
    }
    YACE_WINDOW.get_keyboard().release(tic_tac_keys[position]);

    return true;
}

char tic_tac::get_current_player() const
{
    return current_player_;
}

std::array<cell_state, 9> tic_tac::get_board() const
{
    std::array<cell_state, 9> board{};

    for (size_t i = 0; i < 9; ++i)
        if (chip8_->graphics[priv::chip8_o_positions[i]] == 1)
            board[i] = cell_state::o;
        else if (chip8_->graphics[priv::chip8_x_positions[i]] == 1)
            board[i] = cell_state::x;
        else
            board[i] = cell_state::empty;

    return board;
}

namespace priv
{
    bool is_pressed(ye::chip8 const& chip8, uint32_t const position)
    {
        return chip8.graphics[chip8_o_positions[position]] == 1 || chip8.graphics[chip8_x_positions[position]] == 1;
    }
}

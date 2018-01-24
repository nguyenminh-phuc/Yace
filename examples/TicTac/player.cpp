#include "player.hpp"

#include <algorithm>
#include <random>
#include "tic_tac.hpp"

namespace priv
{
    std::mt19937 mersenne_twister{std::random_device()()};
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
}

player::player(char const name, float const epsilon, float const alpha, float const gamma) :
    name(name),
    epsilon(epsilon),
    alpha(alpha),
    gamma(gamma),
    last_board_({})
{
    last_board_.fill(cell_state::empty);
}

void player::start_game()
{
    last_board_.fill(cell_state::empty);
    last_move_.reset();
}

float player::get_q(std::array<cell_state, 9> const& state, uint32_t const action)
{
    const auto state_action = std::make_tuple(state, action);
    if (q.find(state_action) == q.end())
        q[state_action] = 1.0f; // encourage exploration; "optimistic" 1.0 initial values

    return q[state_action];
}

uint32_t player::move(std::array<cell_state, 9> const& board)
{
    last_board_ = board;
    auto actions = get_available_moves(board);

    if (priv::distribution(priv::mersenne_twister) < epsilon) // explore!
    {
        const std::uniform_int_distribution<size_t> distribution(0, actions.size() - 1);
        last_move_.reset(new uint32_t(actions[distribution(priv::mersenne_twister)]));

        return *last_move_;
    }

    std::map<std::tuple<std::array<cell_state, 9>, uint32_t>, float> qs;
    for (auto const action : actions)
    {
        const auto state_action = std::make_tuple(last_board_, action);
        qs[state_action] = get_q(last_board_, action);
    }

    const auto max_q = (*std::max_element(qs.begin(), qs.end(), qs.value_comp())).second;

    std::vector<uint32_t> best_options;
    for (auto const& qs_element : qs)
    {
        if (qs_element.second == max_q)
        {
            best_options.push_back(std::get<1>(qs_element.first));
        }
    }

    const std::uniform_int_distribution<size_t> distribution(0, best_options.size() - 1);
    last_move_.reset(new uint32_t(best_options[distribution(priv::mersenne_twister)]));

    return *last_move_;
}

void player::set_reward(float const value, std::array<cell_state, 9> const& board)
{
    if (last_move_)
        learn(last_board_, *last_move_, value, board);
}

void player::learn(
    std::array<cell_state, 9> const& state,
    uint32_t const action,
    float const reward,
    std::array<cell_state, 9> const& result_state)
{
    const auto previous_q = get_q(state, action);

    auto actions = get_available_moves(state);
    auto max_q_new = 0.0f;
    for (size_t i = 0; i < actions.size(); ++i)
        if (i == 0)
            max_q_new = get_q(result_state, actions[0]);
        else
        {
            const auto q_new = get_q(result_state, actions[i]);
            if (max_q_new > q_new)
                max_q_new = q_new;
        }

    q[std::make_tuple(state, action)] = previous_q + alpha * ((reward + gamma * max_q_new) - previous_q);
}

std::vector<uint32_t> player::get_available_moves(std::array<cell_state, 9> const& board)
{
    std::vector<uint32_t> moves;

    for (uint32_t i = 0; i < board.size(); ++i)
        if (board[i] == cell_state::empty)
            moves.push_back(i);

    return moves;
}

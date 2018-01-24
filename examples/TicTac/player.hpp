#ifndef TICTAC_PLAYER_HPP
#define TICTAC_PLAYER_HPP

#include <array>
#include <map>
#include <memory>
#include <tuple>
#include <vector>
#include "Yace/yace.hpp"

enum class cell_state;

class player : public ye::non_copyable
{
public:
    player() = delete;

    explicit player(char name, float epsilon = 0.2f, float alpha = 0.3f, float gamma = 0.9f);

    void start_game();

    float get_q(std::array<cell_state, 9> const& state, uint32_t action);

    uint32_t move(std::array<cell_state, 9> const& board);

    void set_reward(float value, std::array<cell_state, 9> const& board);

    void learn(
        std::array<cell_state, 9> const& state,
        uint32_t action, float reward,
        std::array<cell_state, 9> const& result_state);

    char name;

    std::map<std::tuple<std::array<cell_state, 9>, uint32_t>, float> q; // (state, action) keys: Q values

    float epsilon; // e-greedy chance of random exploration

    float alpha; // learning rate

    float gamma; // discount factor for future rewards

private:
    std::array<cell_state, 9> last_board_;

    std::unique_ptr<uint32_t> last_move_;

    static std::vector<uint32_t> get_available_moves(std::array<cell_state, 9> const& board);
};

#endif

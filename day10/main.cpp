#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>

using IntType = std::int64_t;
using CycleLog = std::pair<IntType, IntType>;

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::regex noop_regex("noop");
    std::regex addx_regex("addx ([0-9-]+)");

    IntType sig_strength_acc = 0;
    std::set<IntType> cycles_of_interest = {20, 60, 100, 140, 180, 220};
    auto coi_itr = cycles_of_interest.cbegin();

    IntType cycle_count = 1;
    CycleLog prev_cycle = {0, 1};
    CycleLog new_cycle;
    std::string screen;

    for (std::string cmd; std::getline(input_data, cmd); prev_cycle = new_cycle) {
        std::smatch cmd_match;
        if (std::regex_search(cmd, cmd_match, noop_regex))
            new_cycle = CycleLog(prev_cycle.first + 1, prev_cycle.second);
        else if (std::regex_search(cmd, cmd_match, addx_regex))
            new_cycle = CycleLog(prev_cycle.first + 2, prev_cycle.second + std::atoll(cmd_match.str(1).c_str()));

        // Part 1
        if (prev_cycle.first < *coi_itr && new_cycle.first >= *coi_itr)
            sig_strength_acc += *(coi_itr++) * prev_cycle.second;

        // Part 2
        while (cycle_count - 1 < new_cycle.first) {
            auto sprite_min = prev_cycle.second;
            auto sprite_max = prev_cycle.second + 2;
            auto cursor_pos = ((cycle_count - 1) % 40) + 1;

            if (cursor_pos >= sprite_min && cursor_pos <= sprite_max)
                screen.push_back('#');
            else
                screen.push_back(' ');

            cycle_count++;
        }
    }

    std::cout << "Signal Strength Accumulator: " << sig_strength_acc << std::endl;
    std::cout << "Screen:" << std::endl;
    for (auto line = 0; line < 6; ++line)
        std::cout << screen.substr(line * 40, 40) << std::endl;

    return EXIT_SUCCESS;
}

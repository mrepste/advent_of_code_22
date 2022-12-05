#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <stack>
#include <tuple>
#include <vector>

using IntType = std::int64_t;

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    // Find the line that defines the number of stacks, then set up the stacks themselves
    IntType num_stacks = 0;
    std::stack<std::string> stack_lines;

    for (std::string line; std::getline(input_data, line);) {
        stack_lines.push(line);
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (line[0] == '1') {
            num_stacks = line.length();
            stack_lines.pop();
            break;
        }
    }

    auto get_stack_item = [](const std::string &stack_line, IntType stack_num) -> char {
        IntType offset = 4 * stack_num + 1;
        return (offset > stack_line.length()) ? ' ' : stack_line[offset];
    };

    std::vector<std::stack<char>> stacks(num_stacks);
    while (!stack_lines.empty()) {
        auto stack_line = stack_lines.top();
        for (auto idx = 0; idx < num_stacks; ++idx)
            if (auto item = get_stack_item(stack_lines.top(), idx); item != ' ')
                stacks[idx].push(item);
        stack_lines.pop();
    }

    // Keep two copies of the starting state to handle both parts of the problem
    auto phase1_stacks = stacks;
    auto phase2_stacks = stacks;

    // Execute the moves
    auto extract_moves = [](const std::string &line) -> std::tuple<IntType, IntType, IntType> {
        std::regex regex_expr("[0-9]+");
        std::sregex_iterator regex_itr(line.cbegin(), line.cend(), regex_expr);
        std::sregex_iterator end_itr;

        std::vector<IntType> matches;
        while (regex_itr != end_itr)
            matches.push_back(std::atoll((regex_itr++)->str().c_str()));
        return {matches[0], matches[1] - 1, matches[2] - 1};
    };

    for (std::string line; std::getline(input_data, line);) {
        if (line.empty())
            continue;

        // Phase 1 Moves
        {
            auto [num_to_move, from_stack, to_stack] = extract_moves(line);
            while (num_to_move--) {
                phase1_stacks[to_stack].push(phase1_stacks[from_stack].top());
                phase1_stacks[from_stack].pop();
            }
        }

        // Phase 2 Moves
        {
            auto [num_to_move, from_stack, to_stack] = extract_moves(line);
            std::stack<char> temp_stack;
            while (num_to_move--) {
                temp_stack.push(phase2_stacks[from_stack].top());
                phase2_stacks[from_stack].pop();
            }

            while (!temp_stack.empty()) {
                phase2_stacks[to_stack].push(temp_stack.top());
                temp_stack.pop();
            }
        }
    }

    // Display stack tops
    std::cout << "Phase 1 Stack Tops: ";
    for (const auto &stack : phase1_stacks)
        std::cout << stack.top();
    std::cout << std::endl;

    std::cout << "Phase 2 Stack Tops: ";
    for (const auto &stack : phase2_stacks)
        std::cout << stack.top();
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

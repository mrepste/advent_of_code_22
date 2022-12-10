#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <set>
#include <utility>

using IntType = std::int64_t;

using Coordinate = std::pair<IntType, IntType>;
using Delta = std::pair<IntType, IntType>;

Coordinate move(const Coordinate &c, const Delta &d) { return {c.first + d.first, c.second + d.second}; }
Delta diff(const Coordinate &c2, const Coordinate &c1) { return {c2.first - c1.first, c2.second - c1.second}; }
Delta get_delta_from_text(char c)
{
    switch (c) {
    case 'U':
        return {0, 1};
    case 'D':
        return {0, -1};
    case 'L':
        return {-1, 0};
    case 'R':
        return {1, 0};
    default:
        break;
    }
    return {0, 0};
}

bool within_radius(const Coordinate &c1, const Coordinate &c2, IntType radius)
{
    auto delta_x = std::abs(c2.first - c1.first);
    auto delta_y = std::abs(c2.second - c1.second);
    return delta_x <= radius && delta_y <= radius;
}

void move_rope(std::list<Coordinate> &knots, char head_dir)
{
    // First, move the head
    auto knot_itr = knots.begin();
    *knot_itr = move(*knot_itr, get_delta_from_text(head_dir));

    // For each successive knot, see if it heeds to move. If not, we can stop. If so, figure out the required delta and
    // move it
    auto clamp = [](IntType &val) -> IntType {
        if (std::abs(val) == 2)
            return val > 0 ? 1 : -1;
        return val;
    };

    for (knot_itr++; knot_itr != knots.end(); knot_itr++) {
        if (within_radius(*knot_itr, *std::prev(knot_itr), 1))
            break;

        auto delta = diff(*std::prev(knot_itr), *knot_itr);
        delta.first = clamp(delta.first);
        delta.second = clamp(delta.second);
        *knot_itr = move(*knot_itr, delta);
    }
}

int main(int, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::regex cmd_regex("([UDLR]+) ([0-9]+)$");
    std::list<Coordinate> rope1, rope2;
    std::set<Coordinate> rope1_tail_locations, rope2_tail_locations;

    for (auto idx = 0; idx < 2; ++idx)
        rope1.emplace_back(0, 0);
    for (auto idx = 0; idx < 10; ++idx)
        rope2.emplace_back(0, 0);

    for (std::string cmd; std::getline(input_data, cmd);) {
        std::smatch cmd_match;
        if (std::regex_search(cmd, cmd_match, cmd_regex)) {
            auto times = std::atoll(cmd_match.str(2).c_str());
            while (times--) {
                move_rope(rope1, cmd_match.str(1)[0]);
                rope1_tail_locations.insert(*rope1.rbegin());

                move_rope(rope2, cmd_match.str(1)[0]);
                rope2_tail_locations.insert(*rope2.rbegin());
            }
        }
    }

    std::cout << "Num unique tail locations (2 knots): " << rope1_tail_locations.size() << std::endl;
    std::cout << "Num unique tail locations (10 knots): " << rope2_tail_locations.size() << std::endl;

    return EXIT_SUCCESS;
}

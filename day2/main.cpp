#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <type_traits>

enum class Selection : std::int64_t { ROCK = 0, PAPER = 1, SCISSORS = 2 };
enum class Outcome : std::int64_t { WIN = 6, DRAW = 3, LOSS = 0 };

template <typename EnumType> auto as_integer(EnumType val) -> typename std::underlying_type<EnumType>::type
{
    return static_cast<typename std::underlying_type<EnumType>::type>(val);
}

static std::map<char, Selection> selection_map = {{'A', Selection::ROCK},     {'X', Selection::ROCK},
                                                  {'B', Selection::PAPER},    {'Y', Selection::PAPER},
                                                  {'C', Selection::SCISSORS}, {'Z', Selection::SCISSORS}};

static std::map<char, Outcome> outcome_map = {
    {'X', Outcome::LOSS},
    {'Y', Outcome::DRAW},
    {'Z', Outcome::WIN},
};

static Outcome rps(Selection player, Selection opponent)
{
    return player == opponent ? Outcome::DRAW
                              : ((opponent == Selection((as_integer(player) + 1) % 3)) ? Outcome::LOSS : Outcome::WIN);
}

static Selection get_required_move(Selection opponent, Outcome desired_outcome)
{
    std::int64_t adjustment = desired_outcome == Outcome::WIN ? 1 : (desired_outcome == Outcome::DRAW ? 0 : -1);
    auto required_move = as_integer(opponent) + adjustment;
    return Selection((required_move + (required_move < 0 ? 3 : 0)) % 3);
}

struct Match {
    Match(const std::string &match_details) : first_input(match_details[0]), second_input(match_details[2]) {}

    std::int64_t get_phase1_score() const
    {
        return as_integer(selection_map[second_input]) + 1 +
               as_integer(rps(selection_map[second_input], selection_map[first_input]));
    }

    std::int64_t get_phase2_score() const
    {
        auto our_move = get_required_move(selection_map[first_input], outcome_map[second_input]);
        return as_integer(outcome_map[second_input]) + as_integer(our_move) + 1;
    }

  protected:
    char first_input;
    char second_input;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::list<Match> matches;
    for (std::string line; std::getline(input_data, line);) {
        if (line.size() != 3)
            throw std::runtime_error("Unexpected input format");
        matches.emplace_back(line);
    }

    std::int64_t phase1_total_score = 0;
    std::int64_t phase2_total_score = 0;
    for (const auto &match : matches) {
        phase1_total_score += match.get_phase1_score();
        phase2_total_score += match.get_phase2_score();
    }

    std::cout << "Phase 1 Score: " << phase1_total_score << std::endl;
    std::cout << "Phase 2 Score: " << phase2_total_score << std::endl;

    return EXIT_SUCCESS;
}

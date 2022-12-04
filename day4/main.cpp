#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <string>
#include <utility>
#include <vector>

using IntType = std::int64_t;

std::vector<std::string> tokenize(const std::string &str, char delim)
{
    auto delimiter_search = std::regex(std::string(1, delim));

    std::sregex_token_iterator token_iterator(str.cbegin(), str.cend(), delimiter_search, -1);
    std::sregex_token_iterator end_iterator;

    std::vector<std::string> tokens;
    while (token_iterator != end_iterator) {
        if (token_iterator->length())
            tokens.push_back(*token_iterator);
        token_iterator++;
    }

    return tokens;
}

struct CleaningAssignment {
    using ElfAssignment = std::pair<IntType, IntType>;
    CleaningAssignment(const std::string &assignment_str)
    {
        auto assignments = tokenize(assignment_str, ',');
        auto parse_assignment = [&assignments](IntType elf_index) -> ElfAssignment {
            auto indices = tokenize(assignments[elf_index], '-');
            return {std::atoll(indices[0].c_str()), std::atoll(indices[1].c_str())};
        };

        elf1 = parse_assignment(0);
        elf2 = parse_assignment(1);
    }

    bool completely_overlapping_assignments() const
    {
        return (elf1.first <= elf2.first && elf1.second >= elf2.second) ||
               (elf2.first <= elf1.first && elf2.second >= elf1.second);
    }

    bool partially_overlapping_assignments() const { return elf1.first <= elf2.second && elf2.first <= elf1.second; }

  protected:
    std::pair<IntType, IntType> elf1;
    std::pair<IntType, IntType> elf2;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::list<CleaningAssignment> assignments;
    for (std::string line; std::getline(input_data, line);)
        assignments.emplace_back(line);

    IntType num_complete_overlaps = 0;
    IntType num_partial_overlaps = 0;
    for (auto assignment : assignments) {
        num_complete_overlaps += assignment.completely_overlapping_assignments() ? 1 : 0;
        num_partial_overlaps += assignment.partially_overlapping_assignments() ? 1 : 0;
    }

    std::cout << num_complete_overlaps << " complete overlaps in assignments" << std::endl;
    std::cout << num_partial_overlaps << " partial overlaps in assignments" << std::endl;

    return EXIT_SUCCESS;
}

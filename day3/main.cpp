#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <stdexcept>
#include <string>

static constexpr std::int64_t get_item_priority(char i)
{
    return (i <= 'z' && i >= 'a') ? (i - 'a' + 1) : ((i <= 'Z' && i >= 'A') ? (i - 'A' + 27) : 0);
}

template <typename T> std::set<T> get_set_overlap(const std::set<T> set1, const std::set<T> set2)
{
    std::set<char> overlap;
    std::set_intersection(set1.cbegin(), set1.cend(), set2.cbegin(), set2.cend(),
                          std::inserter(overlap, overlap.begin()));
    return overlap;
}

struct Rucksack {
    Rucksack(const std::string &contents)
    {
        if (contents.size() % 2)
            throw std::runtime_error("Invalid Rucksack contents");
        for (auto i = 0; i < contents.size(); ++i) {
            complete.insert(contents[i]);
            i < (contents.size() / 2) ? first_compartment.insert(contents[i]) : second_compartment.insert(contents[i]);
        }
    }

    std::set<char> operator&(const Rucksack &rhs) const { return get_set_overlap(complete, rhs.complete); }
    std::set<char> operator&(const std::set<char> &rhs) const { return get_set_overlap(complete, rhs); }
    std::set<char> get_compartment_overlap() const { return get_set_overlap(first_compartment, second_compartment); }

  protected:
    std::set<char> complete;
    std::set<char> first_compartment;
    std::set<char> second_compartment;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::list<Rucksack> rucksacks;
    for (std::string line; std::getline(input_data, line);)
        rucksacks.emplace_back(line);

    std::int64_t phase1_accumulator = 0;
    for (const auto &rucksack : rucksacks) {
        auto overlapping_items = rucksack.get_compartment_overlap();
        if (overlapping_items.size() > 1)
            throw std::runtime_error("Too many overlaps");
        phase1_accumulator += get_item_priority(*overlapping_items.begin());
    }

    std::int64_t phase2_accumulator = 0;
    if (rucksacks.size() % 3)
        throw std::runtime_error("Invalid number of rucksacks");
    for (auto rucksack_itr = rucksacks.cbegin(); rucksack_itr != rucksacks.cend(); std::advance(rucksack_itr, 3)) {
        auto overlap = *rucksack_itr & (*std::next(rucksack_itr, 1) & *std::next(rucksack_itr, 2));
        if (overlap.size() > 1)
            throw std::runtime_error("Too many overlaps");
        phase2_accumulator += get_item_priority(*overlap.begin());
    }

    std::cout << "Phase 1: " << phase1_accumulator << std::endl;
    std::cout << "Phase 2: " << phase2_accumulator << std::endl;

    return EXIT_SUCCESS;
}

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

struct Elf {
    Elf(std::size_t id) : id(id), calories(0) {}
    bool operator<(const Elf &rhs) const { return calories < rhs.calories; }
    void add_calories(std::size_t val) { calories += val; }
    std::size_t get_calories() const { return calories; }

    std::string display() const
    {
        std::ostringstream oss;
        oss << "Elf " << id << ": " << calories << " calories";
        return oss.str();
    }

  protected:
    std::size_t id;
    std::size_t calories;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::set<Elf> elves;
    std::size_t elf_index = 1;

    auto active_elf = Elf(elf_index++);
    for (std::string line; std::getline(input_data, line);) {
        if (line.empty()) {
            elves.insert(std::move(active_elf));
            active_elf = Elf(elf_index++);
            continue;
        }
        active_elf.add_calories(std::atoll(line.c_str()));
    }
    elves.insert(std::move(active_elf));

    std::size_t print_index = 0;
    std::size_t running_calorie_sum = 0;
    auto print_max = std::atoi(argv[2]);
    auto elf_itr = elves.crbegin();
    while (print_index++ < print_max && elf_itr != elves.crend()) {
        running_calorie_sum += elf_itr->get_calories();
        std::cout << (elf_itr++)->display() << " (Running Total: " << running_calorie_sum << ")" << std::endl;
    }

    return EXIT_SUCCESS;
}

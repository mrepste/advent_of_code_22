#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <regex>
#include <vector>

using IntType = std::int64_t;
static constexpr IntType PART = 2;

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

struct Monkey {
    std::list<IntType> items;
    std::function<IntType(IntType)> operation;
    std::function<IntType(IntType)> get_outbound_direction;
    std::function<IntType(IntType)> normalize_input;
    IntType total_inspections = 0;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    // Build the monkeys
    std::regex monkey_regex("Monkey ([0-9]+)");
    std::regex item_regex("Starting items: ([0-9, ]+)");
    std::regex operation_regex("Operation: new = old ([\\+\\*]) ([a-z0-9]+)");
    std::regex test_regex("Test: divisible by ([0-9]+)");
    std::regex test_true_regex("If true: throw to monkey ([0-9]+)");
    std::regex test_false_regex("If false: throw to monkey ([0-9]+)");

    std::map<IntType, Monkey> monkeys;
    {
        std::optional<IntType> index;
        std::list<IntType> items;
        std::optional<char> op_char;
        std::optional<std::string> op_val;
        std::optional<IntType> test_divisor;
        std::optional<IntType> test_true;
        std::optional<IntType> test_false;
        IntType normalizer = 1;

        for (std::string line; std::getline(input_data, line);) {
            if (line.empty())
                continue;

            std::smatch match;
            if (std::regex_search(line, match, monkey_regex))
                index = std::atoll(match.str(1).c_str());
            else if (std::regex_search(line, match, item_regex)) {
                auto tokens = tokenize(match.str(1), ',');
                for (auto token : tokens)
                    items.push_back(std::atoll(token.c_str()));
            }
            else if (std::regex_search(line, match, operation_regex)) {
                op_char = match.str(1)[0];
                op_val = match.str(2);
            }
            else if (std::regex_search(line, match, test_regex))
                test_divisor = std::atoll(match.str(1).c_str());
            else if (std::regex_search(line, match, test_true_regex))
                test_true = std::atoll(match.str(1).c_str());
            else if (std::regex_search(line, match, test_false_regex))
                test_false = std::atoll(match.str(1).c_str());

            // If we have the parameters, create the monkey
            if (index && items.size() && op_char && op_val && test_divisor && test_true && test_false) {
                auto &monkey = monkeys[*index];
                monkey.items = items;
                monkey.get_outbound_direction = [test_divisor, test_true, test_false](IntType x) {
                    return (x % *test_divisor) ? *test_false : *test_true;
                };

                if constexpr (PART == 1) {
                    monkey.normalize_input = [](IntType x) { return x; };
                    switch (*op_char) {
                    case '*':
                        if (*op_val == "old")
                            monkey.operation = [](IntType x) { return (x * x) / 3; };
                        else
                            monkey.operation = [op_val](IntType x) { return (x * std::atoll(op_val->c_str())) / 3; };
                        break;
                    case '+':
                        if (*op_val == "old")
                            monkey.operation = [](IntType x) { return (x + x) / 3; };
                        else
                            monkey.operation = [op_val](IntType x) { return (x + std::atoll(op_val->c_str())) / 3; };
                        break;
                    default:
                        break;
                    }
                }
                else {
                    normalizer *= *test_divisor;
                    switch (*op_char) {
                    case '*':
                        if (*op_val == "old")
                            monkey.operation = [](IntType x) { return (x * x); };
                        else
                            monkey.operation = [op_val](IntType x) { return x * std::atoll(op_val->c_str()); };
                        break;
                    case '+':
                        if (*op_val == "old")
                            monkey.operation = [](IntType x) { return (x + x); };
                        else
                            monkey.operation = [op_val](IntType x) { return x + std::atoll(op_val->c_str()); };
                        break;
                    default:
                        break;
                    }
                }

                index.reset();
                items.clear();
                op_char.reset();
                op_val.reset();
                test_divisor.reset();
                test_true.reset();
                test_false.reset();
            }
        }

        for (auto &monkey_pair : monkeys) {
            if constexpr (PART == 1)
                monkey_pair.second.normalize_input = [normalizer](IntType x) { return x; };
            else
                monkey_pair.second.normalize_input = [normalizer](IntType x) { return x % normalizer; };
        }
    }

    // Run the monkey rounds, then sort them
    IntType rounds = PART == 1 ? 20 : 10000;
    while (rounds--) {
        for (auto &monkey_pair : monkeys) {
            auto &monkey = monkey_pair.second;
            for (auto &item : monkey.items) {
                monkey.total_inspections++;
                item = monkey.operation(item);

                auto &inbound_monkey = monkeys[monkey.get_outbound_direction(item)];
                inbound_monkey.items.push_back(inbound_monkey.normalize_input(item));
            }
            monkey.items.clear();
        }
    }

    // Sort the monkeys to find the monkey business
    std::vector<std::pair<IntType, Monkey>> sorted_monkeys;
    for (auto &monkey_pair : monkeys)
        sorted_monkeys.emplace_back(monkey_pair.first, monkey_pair.second);
    std::sort(sorted_monkeys.begin(), sorted_monkeys.end(),
              [](const auto &a, const auto &b) { return a.second.total_inspections > b.second.total_inspections; });

    std::cout << "Monkey Business: "
              << sorted_monkeys[0].second.total_inspections * sorted_monkeys[1].second.total_inspections << std::endl;

    return EXIT_SUCCESS;
}

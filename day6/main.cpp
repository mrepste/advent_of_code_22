#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using IntType = std::int64_t;

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::string msg;
    std::getline(input_data, msg);

    auto chars_until_first_unique_sequence = [&msg](IntType unique_len) -> IntType {
        for (auto start_idx = 0; start_idx < msg.length() - unique_len; ++start_idx) {
            auto test_string = msg.substr(start_idx, unique_len);
            std::sort(test_string.begin(), test_string.end());
            test_string.erase(std::unique(test_string.begin(), test_string.end()), test_string.end());
            if (test_string.length() == unique_len)
                return start_idx + unique_len;
        }
        return -1;
    };

    std::cout << "Part 1: " << chars_until_first_unique_sequence(4) << std::endl;
    std::cout << "Part 2: " << chars_until_first_unique_sequence(14) << std::endl;

    return EXIT_SUCCESS;
}

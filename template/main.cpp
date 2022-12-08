#include <fstream>
#include <iostream>

using IntType = std::int64_t;

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::string msg;
    std::getline(input_data, msg);

    return EXIT_SUCCESS;
}

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <stack>
#include <string>

using IntType = std::int64_t;

struct Directory {

    void add_directory(const std::string &name) { directories[name] = Directory(); }
    void add_file(const std::string &name, IntType size) { files[name] = size; }

    Directory *get_directory(const std::string &name)
    {
        if (auto res = directories.find(name); res != directories.cend()) {
            return &(res->second);
        }
        return nullptr;
    }

    std::ostream &print(std::ostream &os, const std::string &indent = "") const
    {
        static const std::string INDENT = "   ";
        for (const auto &dir_entry : directories) {
            os << indent << "- " << dir_entry.first << " (dir)" << std::endl;
            dir_entry.second.print(os, indent + INDENT);
        }
        for (const auto &file_entry : files)
            os << indent << "- " << file_entry.first << " (size: " << file_entry.second << ")" << std::endl;
        return os;
    }

    std::size_t size() const
    {
        IntType total_size = 0;
        for (const auto &dir_entry : directories)
            total_size += dir_entry.second.size();
        for (const auto &file_entry : files)
            total_size += file_entry.second;
        return total_size;
    }

    void sum_dirs_less_than_size(IntType max_size, IntType &accumulator)
    {
        auto this_dir_size = size();
        accumulator += this_dir_size <= max_size ? this_dir_size : 0;
        for (auto &subdir : directories)
            subdir.second.sum_dirs_less_than_size(max_size, accumulator);
    };

    void get_all_directory_sizes(std::set<IntType> &accumulator)
    {
        accumulator.insert(size());
        for (auto &subdir : directories)
            subdir.second.get_all_directory_sizes(accumulator);
    }

  protected:
    std::map<std::string, Directory> directories;
    std::map<std::string, IntType> files;
};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    Directory root_dir;

    // Parse the input and build out the tree. '$ ls' doesn't mean anything to us.
    std::regex cd_regex("\\$ cd ([\\.\\/A-Za-z]+)");
    std::regex dir_regex("dir ([A-Za-z]+)");
    std::regex file_regex("([0-9]+) ([A-Za-z\\.]+)");

    std::stack<Directory *> dir_stack;
    dir_stack.push(&root_dir);

    for (std::string entry; std::getline(input_data, entry);) {
        std::smatch cmd_match;
        if (std::regex_search(entry, cmd_match, cd_regex)) {
            const auto &arg = cmd_match.str(1);
            if (arg == "/") {
                dir_stack = std::stack<Directory *>();
                dir_stack.push(&root_dir);
            }
            else if (arg == "..")
                dir_stack.pop();
            else {
                if (auto next_dir = dir_stack.top()->get_directory(arg))
                    dir_stack.push(next_dir);
                else
                    throw std::runtime_error("Requested directory not found!");
            }
        }
        else if (std::regex_search(entry, cmd_match, dir_regex)) {
            dir_stack.top()->add_directory(cmd_match.str(1));
        }
        else if (std::regex_search(entry, cmd_match, file_regex)) {
            dir_stack.top()->add_file(cmd_match.str(2), std::atoll(cmd_match.str(1).c_str()));
        }
    }

    // Part 1 -- Find the sum of all directories where (size <= 100000)
    IntType part1 = 0;
    root_dir.sum_dirs_less_than_size(100000, part1);
    std::cout << "Part 1: " << part1 << std::endl;

    // Part 2 -- Find smallest directory to delete
    const IntType space_required = 30000000;
    const IntType filesystem_space = 70000000;
    auto unused_space = filesystem_space - root_dir.size();
    auto deletion_requirement = space_required - unused_space;

    std::set<IntType> part2;
    root_dir.get_all_directory_sizes(part2);
    auto smallest_thing_to_delete = part2.lower_bound(deletion_requirement);
    std::cout << "Part 2: " << *smallest_thing_to_delete << std::endl;

    return EXIT_SUCCESS;
}

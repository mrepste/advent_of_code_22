#include <fstream>
#include <iostream>
#include <vector>

using IntType = std::int64_t;

struct Forest {
    Forest(const std::vector<std::string> &tree_rows)
        : edge_len(tree_rows[0].size()), tree_heights(edge_len * edge_len, 0)
    {
        if (tree_rows.size() != edge_len)
            throw std::runtime_error("Forest not square");
        for (auto row_idx = 0; row_idx < edge_len; ++row_idx) {
            const auto &row = tree_rows[row_idx];
            if (row.size() != edge_len)
                throw std::runtime_error("Inconsistent tree row length");
            auto row_offset = row_idx * edge_len;
            for (auto col_idx = 0; col_idx < edge_len; ++col_idx)
                tree_heights[row_offset + col_idx] = std::atoll(row.substr(col_idx, 1).c_str());
        }
    }

    struct Iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = IntType;
        using pointer = value_type *;
        using reference = value_type &;

        Iterator(std::vector<IntType> *forest_array, IntType start_idx, IntType stride)
            : forest_arr(forest_array), index(start_idx), stride(stride)
        {
        }
        Iterator(const Iterator &) = default;
        Iterator(Iterator &&) = default;
        Iterator &operator=(const Iterator &) = default;
        Iterator &operator=(Iterator &&) = default;

        reference operator*() const { return forest_arr->at(index); }
        pointer operator->() { return forest_arr->data() + index; }
        bool operator==(const Iterator &rhs) const
        {
            return (forest_arr == rhs.forest_arr && index == rhs.index && stride == rhs.stride);
        }
        bool operator!=(const Iterator &rhs) const { return !(*this == rhs); }

        Iterator &operator++()
        {
            index += stride;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator &operator--()
        {
            index -= stride;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

      private:
        std::vector<IntType> *forest_arr;
        IntType index;
        IntType stride;
    };

    IntType get_edge_len() const { return edge_len; }

    Iterator row_itr(IntType row, IntType column) { return Iterator(&tree_heights, edge_len * row + column, 1); }
    Iterator col_itr(IntType row, IntType column) { return Iterator(&tree_heights, edge_len * row + column, edge_len); }
    Iterator row_begin(IntType idx) { return Iterator(&tree_heights, idx * edge_len, 1); }
    Iterator row_end(IntType idx) { return Iterator(&tree_heights, (idx + 1) * edge_len, 1); }
    Iterator col_begin(IntType idx) { return Iterator(&tree_heights, idx, edge_len); }
    Iterator col_end(IntType idx) { return Iterator(&tree_heights, (edge_len * edge_len) + idx, edge_len); }

  protected:
    const IntType edge_len;
    std::vector<IntType> tree_heights;
};

int main(int argc, char *argv[])
{
    // Create the forest
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    std::vector<std::string> tree_rows;
    for (std::string row; std::getline(input_data, row);)
        tree_rows.emplace_back(std::move(row));
    Forest forest(tree_rows);

    auto rev = [](auto iter) { return std::make_reverse_iterator(iter); };

    auto is_visible = [](auto start, auto max) -> bool {
        for (auto pos = std::next(start); pos != max; pos++) {
            if (*start <= *pos)
                return false;
        }
        return true;
    };

    auto get_scenic_score = [](auto start, auto max) -> IntType {
        if (start == max)
            return 0;
        IntType score = 0;
        for (auto pos = std::next(start); pos != max; pos++) {
            score++;
            if (*pos >= *start)
                return score;
        }
        return score;
    };

    IntType visible_count = 0;
    IntType max_scenic_score = 0;
    for (auto row_idx = 0; row_idx < forest.get_edge_len(); ++row_idx) {
        for (auto col_idx = 0; col_idx < forest.get_edge_len(); ++col_idx) {
            if (is_visible(forest.row_itr(row_idx, col_idx), forest.row_end(row_idx)) ||
                is_visible(rev(++forest.row_itr(row_idx, col_idx)), rev(forest.row_begin(row_idx))) ||
                is_visible(forest.col_itr(row_idx, col_idx), forest.col_end(col_idx)) ||
                is_visible(rev(++forest.col_itr(row_idx, col_idx)), rev(forest.col_begin(col_idx)))) {
                visible_count++;
            }

            IntType this_scenic_score =
                get_scenic_score(forest.row_itr(row_idx, col_idx), forest.row_end(row_idx)) *
                get_scenic_score(rev(++forest.row_itr(row_idx, col_idx)), rev(forest.row_begin(row_idx))) *
                get_scenic_score(forest.col_itr(row_idx, col_idx), forest.col_end(col_idx)) *
                get_scenic_score(rev(++forest.col_itr(row_idx, col_idx)), rev(forest.col_begin(col_idx)));
            max_scenic_score = std::max(max_scenic_score, this_scenic_score);
        }
    }

    std::cout << "Visible Count: " << visible_count << std::endl;
    std::cout << "Max Scenic Score: " << max_scenic_score << std::endl;

    return EXIT_SUCCESS;
}

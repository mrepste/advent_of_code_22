#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <set>
#include <vector>

using IntType = std::int64_t;
#define PART 2

struct Node {
    using Path = std::pair<Node *, IntType>;
    Node(IntType height) : height(height) {}

    IntType height;
    IntType index;
    std::vector<Path> transitions;
    bool endpoint = false;

    // Pathfinding state
    IntType node_value = std::numeric_limits<IntType>::max();
    Node *parent = nullptr;
};

struct Coordinate {
    IntType x;
    IntType y;

    Coordinate operator+(const Coordinate &rhs) const { return Coordinate{x + rhs.x, y + rhs.y}; }
};

static constexpr auto R = Coordinate{1, 0};
static constexpr auto L = Coordinate{-1, 0};
static constexpr auto U = Coordinate{0, 1};
static constexpr auto D = Coordinate{0, -1};

int main(int argc, char *argv[])
{
    std::ifstream input_data(argv[1], std::ios::in);
    if (!input_data)
        return EXIT_FAILURE;

    // Read in the terrain and determine the extents
    std::vector<std::string> input_lines;
    for (std::string line; std::getline(input_data, line); input_lines.push_back(line))
        ;
    IntType terrain_max_x = input_lines.front().size();
    IntType terrain_max_y = input_lines.size();

    // Load in the heights
    std::vector<Node> terrain;
    IntType insert_index = 0;
    for (const auto &row : input_lines) {
        for (const auto &elem : row) {
            switch (elem) {
            case 'S': {
                auto &new_node = terrain.emplace_back(0);
                new_node.endpoint = true;
                new_node.node_value = 0;
                break;
            }
            case 'E': {
                auto &new_node = terrain.emplace_back('z' - 'a');
                new_node.endpoint = true;
                break;
            }
            default:
                terrain.emplace_back(elem - 'a');
                break;
            }
            terrain.back().index = insert_index++;
        }
    }

    // Determine the edge transitions. For Part 2, we set any transition between 'a' altitudes to be zero, so that
    // the path to the endpoint will always "start" from the closest 'a' node.
    auto get_node_from_xy = [terrain_max_x, terrain_max_y, &terrain](IntType x, IntType y) -> Node * {
        if (x < 0 || x >= terrain_max_x)
            return nullptr;
        if (y < 0 || y >= terrain_max_y)
            return nullptr;
        return &terrain[y * terrain_max_x + x];
    };

    auto get_coords_from_index = [terrain_max_x, &terrain](IntType idx) -> std::optional<Coordinate> {
        if (idx >= terrain.size())
            return {};
        return {{idx % terrain_max_x, idx / terrain_max_x}};
    };

    for (auto idx = 0; idx < terrain.size(); ++idx) {
        auto coordinate = get_coords_from_index(idx);
        auto node = get_node_from_xy(coordinate->x, coordinate->y);

        for (auto move : {R, L, U, D}) {
            auto neighbor_coordinate = *coordinate + move;
            if (auto neighbor = get_node_from_xy(neighbor_coordinate.x, neighbor_coordinate.y)) {
                if constexpr (PART == 2) {
                    if (neighbor->height == 0 && node->height == 0)
                        node->transitions.emplace_back(neighbor, 0);
                    else if (neighbor->height <= (node->height + 1))
                        node->transitions.emplace_back(neighbor, 1);
                }
                else {
                    if (neighbor->height <= (node->height + 1))
                        node->transitions.emplace_back(neighbor, 1);
                }
            }
        }
    }

    // Run a pathfinding algorithm
    auto node_compare = [](const Node *lhs, const Node *rhs) { return lhs->node_value > rhs->node_value; };
    std::vector<Node *> node_pq;
    std::set<Node *> visited_nodes;

    for (auto &node : terrain)
        node_pq.emplace_back(&node);
    std::sort(node_pq.begin(), node_pq.end(), node_compare);

    while (node_pq.size()) {
        if (node_pq.back()->endpoint && node_pq.back()->node_value)
            break;

        visited_nodes.insert(node_pq.back());
        for (auto edge : node_pq.back()->transitions) {
            if (visited_nodes.count(edge.first))
                continue;
            auto candidate_new_node_val = node_pq.back()->node_value + edge.second;
            if (candidate_new_node_val < edge.first->node_value) {
                edge.first->node_value = candidate_new_node_val;
                edge.first->parent = node_pq.back();
            }
        }
        node_pq.pop_back();
        std::sort(node_pq.begin(), node_pq.end(), node_compare);
    }

    for (auto &node : terrain) {
        if (node.endpoint && node.node_value) {
            std::cout << "Endpoint node has value: " << node.node_value << std::endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}

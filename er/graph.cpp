#include <er/graph.hpp>

#include <fmt/core.h>

namespace ER {

static int longest_name_width()
{
#define O(longname, shortname) #longname,
    static const std::string names[] = {
        NODE_TYPES(O)
        "START",
    };
#undef O
    const auto max = std::max_element(std::begin(names), std::end(names), [](const auto &p, const auto &q) {
        return p.size() < q.size();
    });
    return max->size();
}

static void print_additional_info(const Node &node)
{
    switch (node.type) {
    case Node::Type::CARD:
        fmt::print("    cardinality values: {}:{}",
                   node.info.card.first.to_string(),
                   node.info.card.second.to_string());
        break;
    case Node::Type::GERARCHY:
        fmt::print("    gerarchy type: {}", gerarchy_type_to_string(node.info.gertype));
        break;
    default:
        break;
    }
}

void graph_print(const Graph &graph)
{
    const auto max_width = std::max_element(graph.begin(), graph.end(), [](const auto &p, const auto &q) {
        return p.second.name.size() < q.second.name.size();
    })->second.name.size();
    const auto max_type_width = longest_name_width();
    // const auto max_links_width = std::max_element(graph.begin(), graph.end(), [](const auto &p, const auto &q) {
    //     return p.second.links.size(), < q.second.links.size();
    // })->second.links.size();

    fmt::print("{:3} {:{}} {:{}} links\n", "id", "name", max_width, "type", max_type_width);
    for (const auto &p : graph) {
        fmt::print("{: 3} {:{}} {:{}} [",
                   p.second.id,
                   p.second.name, max_width,
                   node_type_str(p.second.type), max_type_width);
        if (p.second.links.empty())
            fmt::print("None");
        else {
            for (std::size_t i = 0; i < p.second.links.size() - 1; i++)
                fmt::print("{}, ", p.second.links[i]);
            fmt::print("{}", p.second.links[p.second.links.size()-1]);
        }
        fmt::print("]");
        print_additional_info(p.second);
        fmt::print("\n");
    }
}

std::string node_type_str(Node::Type type)
{
#define O(longname, shortname) case Node::Type::longname: return #longname;
    switch (type) {
        NODE_TYPES(O)
        case Node::Type::START: return "START";
        default: return "INVALID";
    }
#undef O
}

} // namespace ER

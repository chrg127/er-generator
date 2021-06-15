#include <er/graph.hpp>

#include <fmt/core.h>

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

void graph_print(const ERGraph &graph)
{
    const auto max_width = std::max_element(graph.begin(), graph.end(), [](const auto &p, const auto &q) {
        return p.second.name.size() < q.second.name.size();
    })->second.name.size();
    const auto max_type_width = longest_name_width();

    for (const auto &p : graph) {
        fmt::print("{:03}: name: {:{}} type: {:{}} links: [",
                   p.second.id,
                   p.second.name, max_width,
                   node_str(p.second.type), max_type_width);
        if (p.second.links.empty())
            fmt::print("None");
        else {
            for (std::size_t i = 0; i < p.second.links.size() - 1; i++)
                fmt::print("{}, ", p.second.links[i]);
            fmt::print("{}", p.second.links[p.second.links.size()-1]);
        }
        fmt::print("]\n");
    }
}

std::string node_str(Node::Type type)
{
#define O(longname, shortname) case Node::Type::longname: return #longname;
    switch (type) {
        NODE_TYPES(O)
        case Node::Type::START: return "START";
        default: return "INVALID";
    }
#undef O
}



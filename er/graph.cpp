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

int links_text_width(const auto &links)
{
    if (links.empty())
        return 4; // "None"
    int w = 0;
    for (int x : links)
        w += 2 + std::to_string(x).size();
    return w;
};

void graph_print(const Graph &graph)
{
    const auto format_links = [](const std::vector<int> &links)
    {
        if (links.empty())
            return std::string("None");
        std::string str;
        str.reserve(links_text_width(links));
        for (std::size_t i = 0; i < links.size() - 1; i++)
            str += fmt::format("{}, ", links[i]);
        return str + fmt::format("{}", links[links.size()-1]);
    };

    const auto format_info = [](const Node &node) -> std::string
    {
        switch (node.type) {
        case Node::Type::CARD:
            return fmt::format("Cardinality values: {}:{}",
                       node.info.card.first.to_string(),
                       node.info.card.second.to_string());
            break;
        case Node::Type::GERARCHY:
            return fmt::format("Gerarchy type: {}", gerarchy_type_to_string(node.info.gertype));
        default:
            return "None";
        }
    };

    const auto max_link_width = [](const Graph &graph)
    {
        auto it = std::max_element(graph.begin(), graph.end(), [&](const auto &p, const auto &q) {
            return links_text_width(p.second.links) < links_text_width(q.second.links);
        });
        return links_text_width(it->second.links) + 2;
    };

    int name_width = std::max_element(graph.begin(), graph.end(), [](const auto &p, const auto &q) {
        return p.second.name.size() < q.second.name.size();
    })->second.name.size();
    int type_width = longest_name_width();
    int links_width = max_link_width(graph);

    fmt::print("{:3} {:{}} {:{}} Anonymous? {:{}} Additional information\n",
               "ID", "Name", name_width, "Type", type_width, "Links", links_width);
    for (const auto &p : graph) {
        fmt::print("{:3} {:{}} {:{}} {:10} {:{}} {}\n",
                   p.second.id,
                   p.second.name, name_width,
                   node_type_str(p.second.type), type_width,
                   p.second.anonymous ? "yes" : "no",
                   "[" + format_links(p.second.links) + "]", links_width,
                   format_info(p.second));
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

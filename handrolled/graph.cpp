#include "graph.hpp"

#include <fmt/core.h>

std::optional<CardinalityValue> CardinalityValue::from_string(std::string_view str)
{
    if (str == "N" || str == "n")
        return CardinalityValue(CARD_MANY);
    else if (auto n = string_convert(str); n)
        return CardinalityValue(n.value());
    return std::nullopt;
}

Cardinality make_card_value(std::string_view str)
{
    auto i = str.find(':');
    auto cv1 = CardinalityValue::from_string(str.substr(0, i));
    auto cv2 = CardinalityValue::from_string(str.substr(i+1, str.size()-i));
    return std::make_pair(*cv1, *cv2);
}


std::string gerarchy_type_to_string(GerarchyType type)
{
    using namespace std::literals;
    return is_subset(type) ? "subset" :
           (is_total(type) ? "total"s : "partial"s) + " "s +
           (is_exclusive(type) ? "exclusive"s : "overlapped"s);
}

std::string node_type_to_string(Node::Type type)
{
#define O(name) case Node::Type::name: return #name;
    switch (type) {
    NODE_TYPES(O)
    default:
        return "ERROR";
    }
#undef O
}

static int longest_name_width()
{
#define O(name) #name,
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

void print_graph(const Graph &graph)
{
    const auto format_links = [](const std::vector<int> &links) {
        if (links.empty())
            return std::string("None");
        std::string str;
        str.reserve(links_text_width(links));
        for (std::size_t i = 0; i < links.size() - 1; i++)
            str += fmt::format("{}, ", links[i]);
        return str + fmt::format("{}", links[links.size()-1]);
    };

    const auto format_info = [](const Node &node) -> std::string {
        std::string info = "";
        if (node.cardinality)
            info += fmt::format("Cardinality values: {}:{}",
                node.cardinality.value().first.to_string(),
                node.cardinality.value().second.to_string());
        if (node.gerarchy_type)
            info += fmt::format("Gerarchy type: {}", gerarchy_type_to_string(node.gerarchy_type.value()));
        return info.empty() ? "None" : info;
    };

    const auto max_link_width = [](const Graph &g) {
        auto it = std::max_element(g.begin(), g.end(), [&](const auto &p, const auto &q) {
            return links_text_width(p.second.links) < links_text_width(q.second.links);
        });
        return it != g.end() ? links_text_width(it->second.links) + 2 : 0;
    };

    int name_width = std::max_element(graph.begin(), graph.end(), [](const auto &p, const auto &q) {
        return p.second.name.size() < q.second.name.size();
    })->second.name.size();
    int type_width = longest_name_width();
    int links_width = max_link_width(graph);

    fmt::print("{:3} {:{}} {:{}} {:{}} Additional information\n",
               "ID", "Name", name_width, "Type", type_width, "Links", links_width);
    for (const auto &p : graph) {
        fmt::print("{:3} {:{}} {:{}} {:{}} {}\n",
                   p.second.id,
                   p.second.name, name_width,
                   node_type_to_string(p.second.type), type_width,
                   "[" + format_links(p.second.links) + "]", links_width,
                   format_info(p.second));
    }
}

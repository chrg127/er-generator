#ifndef ERGRAPH_HPP_INCLUDED
#define ERGRAPH_HPP_INCLUDED

#include <string>
#include <utility>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>

#define NODE_TYPES(O) \
    O(START, start) \
    O(ENTITY, ent) \
    O(ASSOC, assoc) \
    O(GERARCHY, ger) \
    O(ATTR, attr) \
    O(FK, fk) \
    O(PK, pk) \
    O(CARD, card) \
    O(GERTYPE, gertype)

struct Node {
    enum class Type {
#define O(ename, sname) ename,
        NODE_TYPES(O)
#undef O
    } type;
    std::string name;
    std::vector<int> links;
    int id;
};

using ERGraph = std::unordered_map<int, Node>;

inline const ERGraph::const_iterator
graph_find_node(const ERGraph &graph, const std::string &name, Node::Type type)
{
    auto r = std::find_if(graph.begin(), graph.end(), [&](const auto &p) { return p.second.name == name && p.second.type == type; });
    return r;
}

inline const std::vector<int>::const_iterator
graph_find_link(const ERGraph &graph, const Node &node, const std::string &name, Node::Type type)
{
    auto r = std::find_if(node.links.begin(), node.links.end(), [&](int i) {
        auto n = graph.find(i);
        return n->second.name == name && n->second.type == type;
    });
    return r;
}

#endif


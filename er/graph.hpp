#ifndef ERGRAPH_HPP_INCLUDED
#define ERGRAPH_HPP_INCLUDED

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <er/nodeprops.hpp>

namespace ER {

#define NODE_TYPES(O) \
    O(ENTITY, ent) \
    O(ASSOC, assoc) \
    O(GERARCHY, ger) \
    O(ATTR, attr) \
    O(FK, fk) \
    O(PK, pk) \
    O(CARD, card) \

struct Node {
    enum class Type {
        START,
#define O(ename, sname) ename,
        NODE_TYPES(O)
#undef O
    } type;
    std::string name;
    std::vector<int> links;
    int id;
    bool anonymous = false;
    // union for additional info, depending on the node type.
    union {
        Cardinality card;
        GerType gertype;
    } info;

    Node() = default;
    // this is needed to silence a warning about the above union.
    Node(Node::Type t, std::string &&n, std::vector<int> &&l, int i)
        : type(t), name(n), links(l), id(i)
    { }
};

using Graph = std::map<int, Node>;

inline const Graph::const_iterator
graph_find_node(const Graph &graph, const std::string &name, Node::Type type)
{
    auto r = std::find_if(graph.begin(), graph.end(), [&](const auto &p) { return p.second.name == name && p.second.type == type; });
    return r;
}

inline const std::vector<int>::const_iterator
graph_find_link(const Graph &graph, const Node &node, const std::string &name, Node::Type type)
{
    auto r = std::find_if(node.links.begin(), node.links.end(), [&](int i) {
        auto n = graph.find(i);
        return n->second.name == name && n->second.type == type;
    });
    return r;
}

void graph_print(const Graph &graph);
std::string node_type_str(Node::Type type);

} // namespace ER

#endif


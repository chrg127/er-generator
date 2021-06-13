#ifndef ERGRAPH_HPP_INCLUDED
#define ERGRAPH_HPP_INCLUDED

#include <string>
#include <utility>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>

/*
struct Node {
    enum class Type {
        ENTITY,
        ASSOC,
        GERARCHY,
        FK,
    };
    struct EntInfo      { std::vector<int> pkeys; };
    struct AssocInfo    { std::vector<std::pair<int, int>> cards; };
    struct GerarchyInfo { int type; };
    struct ForeignInfo  { std::vector<std::string> refs; };
    struct Attribute    { std::string name; };
    std::string name;
    Type type;
    std::variant<EntInfo, AssocInfo, GerarchyInfo, ForeignInfo> info;
    std::vector<Attribute> attrs;
    std::vector<std::string> links;

    Node() = default;
    Node(Node::Type t) : type(t)
    {
        switch (t) {
        case Type::ENTITY:   info = EntInfo{}; break;
        case Type::ASSOC:    info = AssocInfo{}; break;
        case Type::GERARCHY: info = GerarchyInfo{}; break;
        case Type::FK:       info = ForeignInfo{}; break;
        }
    }
};
*/

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

const ERGraph::const_iterator
graph_find_node(const ERGraph &graph, const std::string &name, Node::Type type)
{
    auto r = std::find_if(graph.begin(), graph.end(), [&](const auto &p) { return p.second.name == name && p.second.type == type; });
    return r;
}

const std::vector<int>::const_iterator
graph_find_link(const ERGraph &graph, const Node &node, const std::string &name, Node::Type type)
{
    auto r = std::find_if(node.links.begin(), node.links.end(), [&](int i) {
        auto n = graph.find(i);
        return n->second.name == name && n->second.type == type;
    });
    return r;
}

#endif


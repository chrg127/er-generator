#include <cassert>
#include <fmt/core.h>
#include "ergraph.hpp"
#include "util.hpp"

int main()
{
    std::vector<Node> nodes;
    nodes.push_back(Node{Node::Type::ENTITY, "hello", {}, 0});
    Node n = std::move(nodes.back());
    nodes.pop_back();
    fmt::print("{} {}\n", n.name, n.id);
}

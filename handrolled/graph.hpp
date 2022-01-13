#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <optional>
#include "util.hpp"

struct CardinalityManyType {};
constexpr inline CardinalityManyType CARD_MANY;

struct CardinalityValue {
    std::optional<int> value;

    CardinalityValue() = default;
    CardinalityValue(int n) : value(n) { }
    CardinalityValue(CardinalityManyType) : value(std::nullopt) { }

    bool operator==(int n) const { return value.value_or(-1) == n; }
    bool operator==(CardinalityManyType) const { return !value.has_value(); }

    std::string to_string() const { return value ? std::to_string(value.value()) : "N"; }
    static std::optional<CardinalityValue> from_string(std::string_view str);
};

using Cardinality = std::pair<CardinalityValue, CardinalityValue>;
Cardinality make_cardinality(std::string_view str);

using GerarchyType = u8;

enum GerarchyFlag {
    GERARCHY_EXCLUSIVE   = 1 << 0,
    GERARCHY_OVERLAPPED  = 0,
    GERARCHY_TOTAL       = 1 << 1,
    GERARCHY_PARTIAL     = 0,
    GERARCHY_SUBSET      = 1 << 2,
};

inline bool is_subset(GerarchyType type)        { return type & GERARCHY_SUBSET; }
inline bool is_total(GerarchyType type)         { return !is_subset(type) && type & GERARCHY_TOTAL; }
inline bool is_partial(GerarchyType type)       { return !is_subset(type) && !is_total(type); }
inline bool is_exclusive(GerarchyType type)     { return !is_subset(type) && type & GERARCHY_EXCLUSIVE; }
inline bool is_overlapped(GerarchyType type)    { return !is_subset(type) && !is_exclusive(type); }

inline GerarchyType make_gerarchy_subset() { return GERARCHY_SUBSET; }
inline GerarchyType make_gerarchy_type(bool total, bool exclusive)
{
    return (total ? GERARCHY_TOTAL : 0) | (exclusive ? GERARCHY_EXCLUSIVE : 0);
}

std::string gerarchy_type_to_string(GerarchyType type);

#define NODE_TYPES(O)   \
    O(Start)            \
    O(Entity)           \
    O(Assoc)            \
    O(Gerarchy)         \
    O(Attr)             \
    O(FK)               \
    O(PK)               \
    O(Card)

struct Node {
    enum class Type {
#define O(name) name,
        NODE_TYPES(O)
#undef O
    } type;
    int id;
    std::string name;
    std::vector<int> links;
    std::optional<Cardinality> cardinality;
    std::optional<GerarchyType> gerarchy_type;
};

std::string node_type_to_string(Node::Type type);

using Graph = std::map<int, Node>;

void print_graph(const Graph &graph);

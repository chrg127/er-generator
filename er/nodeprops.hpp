#ifndef NODEPROPS_HPP_INCLUDED
#define NODEPROPS_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <optional>

/* a structure for cardinality values. a cardinality value is either
 * a non negative integer or the special value N, meaning 'many'. */
struct cardmany_t {};
constexpr inline cardmany_t CARD_MANY;

class CardValue {
    unsigned value = 0; /* = 0 if CARD_MANY */
    bool many = false;

public:
    CardValue() = default;
    CardValue(unsigned n) : value(n) { }
    CardValue(cardmany_t) : many(true) { }

    static std::optional<CardValue> from_string(std::string_view str);

    bool operator==(unsigned n) const { return many ? false : value == n; }
    bool operator==(cardmany_t) const { return many; }

    std::string to_string() const { return many ? "N" : std::to_string(value); }
};

using Cardinality = std::pair<CardValue, CardValue>;

// make a Cardinality value using a string. assumes the string
// is formatted like 0:N, 1:1, N:N, etc.
Cardinality make_card_value(const std::string &str);

#endif

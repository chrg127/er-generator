#ifndef NODEPROPS_HPP_INCLUDED
#define NODEPROPS_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <optional>

namespace ER {

/* a structure for cardinality values. a cardinality value is either
 * a non negative integer or the special value N, meaning 'many'. */
struct cardmany_t {};
constexpr inline cardmany_t CARD_MANY;

struct CardValue {
    unsigned value; /* = 0 if CARD_MANY */
    bool many;

    CardValue() = default;
    CardValue(unsigned n) : value(n), many(false) { }
    CardValue(cardmany_t) : value(0), many(true)  { }

    static std::optional<CardValue> from_string(std::string_view str);

    bool operator==(unsigned n) const { return many ? false : value == n; }
    bool operator==(cardmany_t) const { return many; }

    std::string to_string() const { return many ? "N" : std::to_string(value); }
};

struct Cardinality {
    CardValue first, second;
};

// make a Cardinality value using a string. assumes the string
// is formatted like 0:N, 1:1, N:N, etc.
Cardinality make_card_value(const std::string &str);

/* Gerarchy type handlers.
 * Not exactly type-safe, but as long users only use the provided functions,
 * everything is safe. Also safety isn't very important in this context, y'know?
 */
using GerType = uint8_t;

// 76543210
// 00000111
//        ^ total/partial bit (total: 1, partial: 0)
//       ^ exclusive/overlapped bit (exclusive: 1, overlapped: 0)
//      ^ subset bit
enum GerFlag {
    GERFLAG_EXCLUSIVE   = 1 << 0,
    GERFLAG_OVERLAPPED  = 0,
    GERFLAG_TOTAL       = 1 << 1,
    GERFLAG_PARTIAL     = 0,
    GERFLAG_SUBSET      = 1 << 2,
};

inline bool is_subset(GerType type)        { return type & GERFLAG_SUBSET; }
inline bool is_total(GerType type)         { return !is_subset(type) && type & GERFLAG_TOTAL; }
inline bool is_partial(GerType type)       { return !is_subset(type) && !is_total(type); }
inline bool is_exclusive(GerType type)     { return !is_subset(type) && type & GERFLAG_EXCLUSIVE; }
inline bool is_overlapped(GerType type)    { return !is_subset(type) && !is_exclusive(type); }

inline GerType make_gerarchy_subset() { return GERFLAG_SUBSET; }
inline GerType make_gerarchy_type(bool total, bool exclusive) { return (total ? GERFLAG_TOTAL : 0) | (exclusive ? GERFLAG_EXCLUSIVE : 0); }

std::string gerarchy_type_to_string(GerType type);

} // namespace ER

#endif

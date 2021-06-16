#include "nodeprops.hpp"

#include <cassert>
#include <er/util.hpp>

std::optional<CardValue> CardValue::from_string(std::string_view str)
{
    if (str == "N" || str == "n")
        return CardValue(CARD_MANY);
    else if (auto n = util::strconv(str); n)
        return CardValue(n.value());
    return std::nullopt;
}

Cardinality make_card_value(const std::string &str)
{
    auto i = str.find(':');
    assert(i != str.npos && "this should never happen");
    auto cv1 = CardValue::from_string({str.data(), i});
    auto cv2 = CardValue::from_string({str.data() + i + 1, str.size() - i - 1});
    assert(cv1 && cv2 && "this should never happen");
    return std::make_pair(*cv1, *cv2);
}


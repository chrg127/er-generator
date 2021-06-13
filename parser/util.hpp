#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED

#include <cstdio>
#include <cstring>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace util {

/* Converts a string to a number. */
template <typename T = int>
std::optional<T> _strconv(const char *start, const char *end, unsigned base = 10)
{
    static_assert(std::is_integral_v<T>, "T must be an integral numeric type");
    T value = 0;
    auto res = std::from_chars(start, end, value, base);
    if (res.ec != std::errc() || res.ptr != end)
        return std::nullopt;
    return value;
}

template <typename T = int, typename TStr = std::string>
std::optional<T> strconv(const TStr &str, unsigned base = 10)
{
    if constexpr(std::is_same<std::decay_t<TStr>, char *>::value)
        return _strconv<T>(str, str + std::strlen(str), base);
    else
        return _strconv<T>(str.data(), str.data() + str.size(), base);
}

// Check for space, no locale support.
inline bool is_space(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

template <typename T>
std::string tostr(const T &x)
{
    if constexpr(std::is_same<T, std::string>::value)
        return x;
    else if constexpr(std::is_same<std::decay_t<T>, char*>::value)
        return std::string(x);
    else if constexpr(std::is_integral_v<T>)
        return std::to_string(x);
    else
        return "error";
}

template <typename... T>
std::string concat(const T&... args)
{
    return (tostr(args) + ...);
}

/*
template <typename K, typename V>
V &map_find(const std::unordered_map<K, V> &map, const K &key)
{
    auto it = map.find(key);
    if (it != map.end())
        throw std::runtime_error("");
    return it->
}
*/

std::string read_all(const char *pathname)
{
    std::string str;
    FILE *f = fopen(pathname, "r");

    fseek(f, 0L, SEEK_END);
    long size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    str.reserve(size);
    for (int c; c = fgetc(f), c != EOF; )
        str += c;
    fclose(f);
    return str;
}

// template <typename It>
// std::string remove_comments(It first, It last)
// {
//     std::string res;
//     res.reserve(last - first);

//     while (first != last) {
//         if (*first == ';')
//             first = std::find_if(first, last, [](char c) { return c == '\n'; });
//         else
//             res += *first++;
//     }

//     return res;
// }


} // namespace util

#endif

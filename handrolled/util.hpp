#pragma once

#include <cstdio>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>
#include <optional>
#include <charconv>

using u32 = uint32_t;
using u8  = uint8_t;
using i32 = int32_t;
using i8  = int8_t;

inline std::optional<std::string> file_to_string(std::string_view pathname)
{
    FILE *f = fopen(pathname.data(), "r");
    if (!f) {
        perror("error:");
        return std::nullopt;
    }
    fseek(f, 0, SEEK_END);
    auto size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::string text(size, ' ');
    fread(text.data(), sizeof(char), size, f);
    fclose(f);
    return text;
}

inline bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-'; }
inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
inline bool is_space(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

template <typename T = int>
std::optional<T> _string_convert_helper(const char *start, const char *end, unsigned base = 10)
{
    static_assert(std::is_integral_v<T>, "T must be an integral numeric type");
    T value = 0;
    auto res = std::from_chars(start, end, value, base);
    if (res.ec != std::errc() || res.ptr != end)
        return std::nullopt;
    return value;
}

template <typename T = int, typename TStr = std::string>
std::optional<T> string_convert(const TStr &str, unsigned base = 10)
{
    if constexpr(std::is_same<std::decay_t<TStr>, char *>::value)
        return _string_convert_helper<T>(str, str + std::strlen(str), base);
    else
        return _string_convert_helper<T>(str.data(), str.data() + str.size(), base);
}

template <typename T>
inline std::string _concat_helper(const T &x)
{
         if constexpr(std::is_same<T, std::string>::value)         return x;
    else if constexpr(std::is_same<std::decay_t<T>, char*>::value) return std::string(x);
    else if constexpr(std::is_integral_v<T>)                       return std::to_string(x);
    else                                                           return "error";
}

inline std::string concat(const auto&... args)
{
    return (_concat_helper(args) + ...);
}

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>

#define TOKEN_TYPES(O) \
    O(LeftParen)        O(RightParen)       O(Ident)            O(Number)           \
    O(End)              O(Error)            O(Entity)           O(Attr)             \
    O(PK)               O(FK)               O(Assoc)            O(Between)          \
    O(Card)             O(Gerarchy)         O(Type)             O(Subset)           \
    O(Partial)          O(Total)            O(Exclusive)        O(Overlapped)       \
    O(Parent)           O(Child)

#define O(name) name,
struct Token {
    enum class Type {
        TOKEN_TYPES(O)
    } type = Type::Error;
    std::string_view text;
    size_t pos = 0;
};
#undef O

std::string_view token_type_to_string(Token::Type t);
std::pair<size_t, size_t> token_position(Token t, std::string_view text);

struct Lexer {
    std::string_view text;
    size_t start = 0;
    size_t cur = 0;

    Lexer(std::string_view s) : text(s) { }

    std::vector<Token> lex();
    Token lex_one();

    char peek() const               { return text[cur]; }
    char peek_next() const          { return text[cur+1]; }
    char advance()                  { return text[cur++]; }
    bool at_end() const             { return text.size() == cur; }
    auto position_of(Token t) const { return token_position(t, text); }

    Token make(Token::Type type);
    Token error(std::string_view msg);
    bool match(char expected);
    void skip_whitespace();
    bool is_cardinality_value(char c);

    Token ident();
    Token cardinality(char start);
    Token::Type get_ident_type();
    Token::Type check_keyword(size_t start, std::string_view rest, Token::Type type);
    Token::Type check_two_keywords(size_t start, std::string_view first, std::string_view second, Token::Type type);
};

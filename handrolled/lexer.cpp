#include "lexer.hpp"

#include "util.hpp"

std::string_view token_type_to_string(Token::Type t)
{
    switch (t) {
#define O(name) case Token::Type::name: return #name;
    TOKEN_TYPES(O)
#undef O
    default: return "ERROR";
    }
}

std::pair<size_t, size_t> token_position(Token t, std::string_view text)
{
    auto tmp = text.substr(0, t.pos);
    auto line = std::count(tmp.begin(), tmp.end(), '\n') + 1;
    auto last_nl = tmp.find_last_of('\n');
    auto column = t.pos - last_nl;
    return std::make_pair(line, column);
}

Token Lexer::make(Token::Type type)
{
    return (Token) {
        .type = type,
        .text = text.substr(start, cur - start),
        .pos  = start,
    };
}

Token Lexer::error(std::string_view msg)
{
    return (Token) {
        .type = Token::Type::Error,
        .text = msg,
        .pos  = start,
    };
}

bool Lexer::match(char expected)
{
    if (at_end() || text[cur] != expected)
        return false;
    cur++;
    return true;
}

void Lexer::skip_whitespace()
{
    for (;;) {
        switch (peek()) {
        case ' ': case '\r': case '\t': case '\n':
            advance();
            break;
        case ';':
            while (peek() != '\n')
                advance();
            break;
        default:
            return;
        }
    }
}

Token Lexer::ident()
{
    while (is_alpha(peek()) || is_digit(peek()))
        advance();
    return make(get_ident_type());
}

Token::Type Lexer::check_keyword(size_t st, std::string_view rest, Token::Type type)
{
    return (cur - start == st + rest.size() && rest == text.substr(start+st, rest.size()))
        ? type
        : Token::Type::Ident;
}

Token::Type Lexer::check_two_keywords(size_t st, std::string_view first, std::string_view second, Token::Type type)
{
    return (cur - start == st + first.size()  && first  == text.substr(start+st, first.size()))
        || (cur - start == st + second.size() && second == text.substr(start+st, second.size()))
        ? type
        : Token::Type::Ident;
    // auto rest = text.substr(start, cur-start);
    // return ((cur-start == start+first.size() && first == rest)
    //     ||  (cur-start == start+second.size() && rest == rest)) ? type : Token::Type::Ident;
}

Token::Type Lexer::get_ident_type()
{
    auto size = cur - start;
    auto word = text.substr(start, text.size() - start);
    switch (word[0]) {
    case 'a':
        if (size > 1) {
            switch (word[1]) {
            case 't': return check_two_keywords(2, "tr", "tribute", Token::Type::Attr);
            case 's': return check_two_keywords(2, "soc", "sociation", Token::Type::Assoc);
            }
        }
        break;
    case 'b': return check_keyword(1, "etween", Token::Type::Between);
    case 'c': return check_keyword(1, "hild", Token::Type::Child);
    case 'e':
        if (size > 1) {
            switch (word[1]) {
            case 'n': return check_keyword(2, "tity", Token::Type::Entity);
            case 'x': return check_keyword(2, "clusive", Token::Type::Exclusive);
            }
        }
              return check_keyword(1, "ntity", Token::Type::Entity);
    case 'f':
        if (size > 1) {
            switch (word[1]) {
            case 'k': return size == 2 ? Token::Type::PK : Token::Type::Ident;
            case 'o': return check_keyword(2, "reign-key", Token::Type::FK);
            }
        }
        break;
    case 'g': return check_keyword(1, "erarchy", Token::Type::Gerarchy);
    case 'o': return check_keyword(1, "verlapped", Token::Type::Overlapped);
    case 'p':
        if (size > 1) {
            switch (word[1]) {
            case 'k': return size == 2 ? Token::Type::PK : Token::Type::Ident;
            case 'r': return check_keyword(2, "imary-key", Token::Type::PK);
            case 'a':
                if (size > 3 && word[2] == 'r') {
                    switch (word[3]) {
                    case 't': return check_keyword(4, "ial", Token::Type::Partial);
                    case 'e': return check_keyword(4, "nt",  Token::Type::Parent);
                    }
                }
            }
        }
        break;
    case 's': return check_keyword(1, "ubset", Token::Type::Subset);
    case 't':
        if (size > 1) {
            switch (word[1]) {
            case 'o': return check_keyword(2, "tal", Token::Type::Total);
            case 'y': return check_keyword(2, "pe",  Token::Type::Type);
            }
        }
    }
    return Token::Type::Ident;
}

bool Lexer::is_cardinality_value(char c)
{
    return is_digit(c) || ((c == 'n' || c == 'N') && !is_alpha(peek()));
}

Token Lexer::cardinality(char start)
{
    if (start != 'n' && start != 'N')
        while (is_digit(peek()))
            advance();
    return make(Token::Type::Card);
}

Token Lexer::lex_one()
{
    skip_whitespace();
    start = cur;
    if (at_end())
        return make(Token::Type::End);

    char c = advance();
    switch (c) {
    case '(': return make(Token::Type::LeftParen);
    case ')': return make(Token::Type::RightParen);
    }
    if (is_cardinality_value(c))
        return cardinality(c);
    if (is_alpha(c))
        return ident();

    return error("unexpected character");
}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> v;
    Token t;
    do {
        t = lex_one();
        v.push_back(t);
    } while (t.type != Token::Type::End);
    return v;
}

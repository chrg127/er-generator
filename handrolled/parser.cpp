#include "parser.hpp"

#include <fmt/core.h>

using enum Token::Type;

std::optional<Graph> Parser::parse()
{
    nodes.push(Node{Node::Type::Start, id++, "start", {}, std::nullopt, std::nullopt});
    scopes.push_back({});
    advance();
    while (!at_end())
        top_level();
    if (had_error)
        return std::nullopt;
    pop_node();
    return graph;
}

void Parser::advance()
{
    prev = cur;
    Token t;
    while (t = next_token(), t.type == Error) {
        auto [line, col] = lexer->position_of(t);
        fmt::print(stderr, "{}:{}: parse error: {}\n", line, col, t.text);
    }
    cur = t;
    switch (prev.type) {
    case LeftParen:  parens++; break;
    case RightParen: parens--; break;
    default: ;
    }
}

void Parser::consume(Token::Type type, std::string_view msg)
{
    if (check(type)) {
        advance();
        return;
    }
    error_curr(msg);
}

bool Parser::match(Token::Type type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}

void Parser::error_at(Token token, std::string_view msg)
{
    had_error = true;
    auto [line, col] = lexer->position_of(token);
    auto err_msg = fmt::format("{}:{}: parse error{}: {}",
        line, col,
          token.type == End   ? " on end of file"
        : token.type == Error ? ""
        : fmt::format(" at '{}'", token.text),
        msg);
    throw ParseError(err_msg);
}

void Parser::sync()
{
    while (cur.type != End) {
        advance();
        if (parens == 0)
            return;
    }
}

void Parser::push_node(Node::Type type, std::string_view name)
{
    auto r = scopes.back().emplace(name, Identifier{id, type});
    if (!r.second)
        error(fmt::format("duplicate definition of {}", name));
    add_link(id);
    nodes.push(Node{type, id++, std::string{name}, {}, std::nullopt, std::nullopt});
    scopes.push_back({});
}

void Parser::pop_node()
{
    scopes.pop_back();
    auto node = nodes.top();
    graph[node.id] = node;
    nodes.pop();
}

int Parser::find_name_in(const auto &scope, std::string_view name, Node::Type type)
{
    if (auto i = scope.find(name); i != scope.end() && i->second.type == type)
        return i->second.id;
    return -1;
}

int Parser::find_name(std::string_view name, Node::Type type)
{
    for (auto scope = scopes.crbegin(); scope != scopes.crend(); ++scope)
        if (int id = find_name_in(*scope, name, type); id != -1)
            return id;
    error(fmt::format("invalid reference for identifier {} for type {}", name, node_type_to_string(type)));
}

int Parser::find_attr(int entity_id, std::string_view name)
{
    Node &entity = graph[entity_id];
    for (auto id : entity.links) {
        Node &attr = graph[id];
        if (attr.type == Node::Type::Attr && attr.name == name)
            return id;
    }
    error(fmt::format("identifier {} not found", name));
}

static const std::vector<Parser::Field> fields_tab[] = {
    /* top level */ { { Entity, &Parser::entity }, { Assoc,  &Parser::association }, { Gerarchy, &Parser::gerarchy }, { FK, &Parser::foreign_key } },
    /* entity */    { { Attr, &Parser::attr },     { PK, &Parser::primary_key } },
    /* assoc */     { { Attr, &Parser::attr },     { Entity, &Parser::assoc_branch } },
    /* gerarchy */  { { Parent, &Parser::parent }, { Child, &Parser::child } },
    /* fk */        { { Attr, &Parser::attr_ref }, { Between, &Parser::entity_ref }, { Assoc, &Parser::assoc_ref } },
    /* attr */      { { Attr, &Parser::attr } }
};

void Parser::parse_field(const auto &fields)
{
    consume(LeftParen, "expected left paren");
    for (auto &field : fields) {
        if (match(field.type)) {
            (this->*field.function)();
            return;
        }
    }
    error("unrecognized field");
}

void Parser::parse_object(Node::Type type, std::string_view name, int fields_index, auto &&other_fields)
{
    consume(Ident, fmt::format("expected {} name", name));
    push_node(type, prev.text);
    other_fields();
    while (!check(RightParen) && !check(End))
        parse_field(fields_tab[fields_index]);
    consume(RightParen, "expected right paren");
    pop_node();
}

void Parser::top_level()
{
    try {
        parse_field(fields_tab[0]);
    } catch (const ParseError &error) {
        fmt::print(stderr, "{}\n", error.what());
        while (nodes.size() != 1)
            nodes.pop();
        sync();
    }
}

void Parser::entity()       { parse_object(Node::Type::Entity,   "entity",      1, [](){}); }
void Parser::association()  { parse_object(Node::Type::Assoc,    "association", 2, [](){}); }
void Parser::gerarchy()     { parse_object(Node::Type::Gerarchy, "gerarchy",    3, [&](){ curr().gerarchy_type = gerarchy_type(); }); }
void Parser::foreign_key()  { parse_object(Node::Type::FK,       "foreign_key", 4, [](){}); }
void Parser::attr()         { parse_object(Node::Type::Attr,     "attribute",   5, [&](){ if (check(Card)) { advance(); cardinality(); } }); }

void Parser::primary_key()
{
    std::vector<int> links;
    while (!check(RightParen) && !check(End)) {
        consume(Ident, "expected identifier");
        links.push_back(find_name_in(scopes.back(), prev.text, Node::Type::Attr));
    }
    consume(RightParen, "expected right paren");
    push_node(Node::Type::PK, anonymous_name("pk"));
    curr().links = std::move(links);
    pop_node();
}

void Parser::assoc_branch()
{
    consume(Ident, "expected identifier");
    int id = find_name(prev.text, Node::Type::Entity);
    consume(Card, "expected cardinality value");
    push_node(Node::Type::Card, anonymous_name("card"));
    cardinality();
    add_link(id);
    pop_node();
    consume(RightParen, "expected right paren");
}

GerarchyType Parser::gerarchy_type()
{
    if (check(Subset)) {
        advance();
        return make_gerarchy_subset();
    }
    advance();
    auto t1 = prev.type;
    if (t1 != Total && t1 != Partial)
        error("expected type 'subset', 'total' or 'exclusive' for gerarchy");
    advance();
    auto t2 = prev.type;
    if (t2 != Exclusive && t2 != Overlapped)
        error("expected type 'exclusive' or 'overlapped' for gerarchy");
    return make_gerarchy_type(t1 == Total, t2 == Exclusive);
}

void Parser::reference_of(Node::Type type)
{
    consume(Ident, "expected identifier");
    add_link(find_name(prev.text, type));
    consume(RightParen, "expected right paren");
}

void Parser::attr_ref()
{
    consume(Ident, "expected identifier");
    auto attr_name = prev.text;
    consume(Ident, "expected identifier");
    add_link(find_attr(find_name(prev.text, Node::Type::Entity), attr_name));
    consume(RightParen, "expected right paren");
}

void Parser::cardinality()
{
    auto v1 = CardinalityValue::from_string(prev.text).value();
    consume(Card, "expected cardinality value");
    auto v2 = CardinalityValue::from_string(prev.text).value();
    nodes.top().cardinality = std::make_pair(v1, v2);
}

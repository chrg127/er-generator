#pragma once

#include <span>
#include <stack>
#include "lexer.hpp"
#include "graph.hpp"
#include "util.hpp"

class Parser {
    struct Identifier {
        Node::Type type;
        std::string_view name;
        bool operator==(const Identifier &other) const { return type == other.type && name == other.name; }
    };

    struct IdentifierHash {
        IdentifierHash() = default;
        size_t operator()(const Identifier &key) const { return std::hash<std::string_view>()(key.name); }
    };

    struct ParseError : std::runtime_error {
        using std::runtime_error::runtime_error;
        using std::runtime_error::what;
    };

    Lexer *lexer;
    Token cur, prev;
    bool had_error = false;
    int parens = 0;
    int id = 0;
    Graph graph;
    std::stack<Node> nodes;
    std::vector<std::unordered_map<Identifier, int, IdentifierHash>> scopes;

public:
    struct Field {
        Token::Type type;
        void (Parser::*function)();
    };

    Parser(Lexer *l) : lexer(l) { }

    std::optional<Graph> parse();
    void advance();
    void consume(Token::Type type, std::string_view msg);
    bool match(Token::Type type);
    bool check(Token::Type type) { return cur.type == type; }

    void error_at(Token token, std::string_view msg);
    void sync();
    void error(std::string_view msg)      { error_at(prev, msg); }
    void error_curr(std::string_view msg) { error_at(cur,  msg); }

    void push_node(Node::Type type, std::string_view name);
    void pop_node();
    int find_name_in(const auto &scope, std::string_view name, Node::Type type);
    int find_name(std::string_view name, Node::Type type);
    int find_attr(int entity_id, std::string_view name);
    bool find_type_in(const auto &scope, Node::Type type);
    Node & curr()                                   { return nodes.top(); }
    auto & curr_scope()                             { return scopes.back(); }
    void add_link(int id)                           { curr().links.push_back(id); }
    std::string anonymous_name(const auto&... args) { return std::to_string(id) + "_" + concat(args...); }

    void parse_field(const auto &fields);
    void parse_object(Node::Type type, std::string_view name, int fields_index, auto &&other_fields);
    void top_level();
    void entity();
    void association();
    void gerarchy();
    void foreign_key();
    void attr();
    void primary_key();
    void assoc_branch();
    void reference_of(Node::Type type);
    void attr_ref();
    GerarchyType gerarchy_type();
    Cardinality cardinality();
    void parent()     { reference_of(Node::Type::Entity); }
    void child()      { reference_of(Node::Type::Entity); }
    void entity_ref() { reference_of(Node::Type::Entity); }
    void assoc_ref()  { reference_of(Node::Type::Assoc); }
};

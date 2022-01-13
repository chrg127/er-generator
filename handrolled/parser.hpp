#pragma once

#include <span>
#include <stack>
#include "lexer.hpp"
#include "graph.hpp"
#include "util.hpp"

class Parser {
    struct Identifier {
        int id;
        Node::Type type;
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
    std::vector<std::unordered_map<std::string_view, Identifier>> scopes;

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
    Token next_token()           { return lexer->lex_one(); }
    bool at_end()                { return lexer->at_end(); }
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
    Node & curr()                                   { return nodes.top(); }
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
    GerarchyType gerarchy_type();
    void reference_of(Node::Type type);
    void attr_ref();
    void cardinality();

    void parent()     { reference_of(Node::Type::Entity); }
    void child()      { reference_of(Node::Type::Entity); }
    void entity_ref() { reference_of(Node::Type::Entity); }
    void assoc_ref()  { reference_of(Node::Type::Assoc); }
};

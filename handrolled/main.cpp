#include <fmt/core.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "util.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fmt::print("usage: {} [file]\n", *argv);
        return 1;
    }

    auto text = file_to_string(argv[1]);
    Lexer lexer{text.value()};
    Parser parser{&lexer};
    auto graph = parser.parse();
    if (graph)
        print_graph(graph.value());

    return 0;
}

#include <cstdio>
#include <string>
#include <fmt/core.h>
#include <er/graph.hpp>
#include <er/parser/parser.hpp>

inline std::string read_all(const char *pathname)
{
    std::string str;
    FILE *f = fopen(pathname, "r");
    if (!f) {
        fmt::print(stderr, "error: couldn't open file: ");
        std::perror("");
        return "";
    }

    fseek(f, 0L, SEEK_END);
    long size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    str.reserve(size);
    for (int c; c = fgetc(f), c != EOF; )
        str += c;
    fclose(f);
    return str;
}

ERGraph parse_file(const std::string &infile, const std::string &outfile, const std::string &contents)
{
    LexContext ctx{ infile, outfile, contents };
    yy::ERParser parser{ctx};
    parser.parse();
    return ctx.getgraph();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fmt::print(stderr, "usage: erlisp [filename]\n");
        return 1;
    }
    std::string contents = read_all(argv[1]);
    if (contents.empty())
        return 1;
    std::string filename = argv[1];
    std::string output = "output.txt";
    ERGraph graph = parse_file(filename, output, contents);
    graph_print(graph);
}


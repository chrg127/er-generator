#include <string>
#include <fmt/core.h>
#include <er/ergraph.hpp>
#include <er/parser/parser.hpp>

ERGraph parse_file(const std::string &infile, const std::string &outfile, const std::string &contents)
{
    LexContext ctx;
    ctx.cursor = contents.c_str();
    ctx.loc.begin.filename = &infile;
    ctx.loc.end.filename = &outfile;

    yy::ERParser parser{ctx};
    parser.parse();
    return ctx.graph;
}

int main(int argc, char *argv[])
{
    if (argc != 2) return 1;
    std::string filename = argv[1];
    std::string contents = util::read_all(argv[1]);
    std::string output = "output.txt";
    ERGraph graph = parse_file(filename, output, contents);
    for (const auto &p : graph) {
        fmt::print("id: {} name: {} links: \n", p.second.id, p.second.name);
    }
}


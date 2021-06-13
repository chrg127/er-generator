bison parser/erlisp.ypp -o parser/erlisp.cpp.re
re2c parser/erlisp.cpp.re -o parser/erlisp.cpp
g++ -g -std=c++20 parser/erlisp.cpp -o erlisp -lfmt

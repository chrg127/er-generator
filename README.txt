=== er-generator ===

An Entity-Relationship diagram generator. This generator uses a text file to
generate an image or something else of the diagram described in the file.

=== The language ===

The language is a LISP-looking language made just for this program. It, however,
is not a programming language: it's more like XML or JSON, but it's built
specifically to describe diagrams.

For example, to declare an entity with some attributes, one would write:

    (entity user (attr id) (attr name) (pk id))

Or an association:

    (assoc assoc-example
        (between user product)
        (cardinality user 0 N)
        (cardinality product 1 N)

=== The program itself ===

To compile the program, simply run `make` in the root directory. You will get a
binary called `erlisp` in the `debug` directory.
You run the program like you'd run a compiler:

    erlisp mydiagram.txt

A bunch of examples can be found in the test/ directory.

Note that the program is incomplete. The only complete part is the parser, which
simply outputs the diagram graph to stdout.


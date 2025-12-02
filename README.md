# `calc`

`calc` is a command-line scientific calculator written in `C++`. It is meant to
enable calculating arbitrary nested mathematical expressions in a one-liner,
*ie.* without entering a separate user interface. It can also optionally show
intermediate calculation steps and display results with a custom precision.

## Installation

Building the binary for this program requires that `cmake` and GNU `make` are
installed.

1. Clone this repository
2. Enter `calc/build/`
3. Run `cmake .. -DCMAKE_BUILD_TYPE=Release`
4. Run `make calc`
5. If the executable doesn't have execute permissions, run for example
    `chmod u+x ./calc`
6. If you want to be able to run `calc` anywhere, move it to somewhere on your
    `PATH`.

## Usage

```bash
calc [-h|--help] [-p|--precision <num_digits>] [-v|--verbose] <expression_args>
```

### Options

- `-p|--precision <num_digits>`: Set number of digits to display in final
    result to `<num_digits>` except trailing zeros. Defaults to 6
- `-v|--verbose`: Print each step in calculation of the expression
- `-h|--help`: Display the command help

### Arguments

You can pass any number of arguments into `calc` which, when concatenated, form
a mathematical expression. Whitespace is ignored, and wrapping the expression 
in quotes is unnecessary if one avoids characters like `*` which may lead the
terminal to attempt glob expansion or `(` `)` which may lead to command
substitution in some shells. If in doubt, use quotes.

`calc` supports parentheses and several mathematical functions and operators,
including the binary operators `+`, `-`, `*` or `x` (multiplication),
`/` (division), `%` (modulo), and `^` (exponent). It currently supports the
following functions:
- `sqrt()`
- `sin()`, `cos()`, `tan()`, `sinh()`, `cosh()`, and `tanh()`
- `e^()` or  `exp()` (exponent of Euler's number)
- `ln()` (natural logarithm) and `log()` (base 10 logarithm)

### Examples

```bash
> calc 1 + 2 x 3
7
> calc -p 8 "ln(3) + 3^2*sin(2.3)*cos(1.2)^2"
1.9798332
> calc -v "-ln(4*3)^2+(9-2+3)"
-ln(4*3)^2+(9-2+3)
-1x2.48491^2+10
Result: 3.82524
> calc -p 3 -v "cos(sqrt(2)^(2/3))"
cos(sqrt(2)^(2/3))
cos(1.26)
Result: 0.306
```

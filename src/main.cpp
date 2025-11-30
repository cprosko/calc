#include <iomanip>
#include <iostream>

#include "ArgParser.h"
#include "Expression.h"

static constexpr std::string_view helpStr{
    "\
calc: Calculate a mathematical expression.\n\
\n\
Usage: calc [-h|--help] [-p|--precision <num_digits>] <expression_args>\n\
\n\
Options:\n\
  -p|--precision <num_digits>: Set number of digits after decimal to display\n\
    in final result to <num_digits>.\n\
  -h|--help: Display this help string\n\
Arguments:\n\
  <expression_args>: Any number of arguments which, when concatenated,\n\
    produce a mathematical expression to be evaluated.\n\
\n\
    Whitespace is ignored, and wrapping the expression in quotes is \n\
    unnecessary if one avoids characters like '*' which may lead the terminal\n\
    to attempt glob expansion or '('/')' which may lead to command\n\
    substitution in some shells.\n\
\n\
    Supports parentheses and several mathematical functions and operators,\n\
    including the binary operators:\n\
      +, -, * or x (multiplication), / (division), % (modulo), ^ (exponent)\n\
    and the functions:\n\
      sqrt(), sin(), cos(), tan(), sinh(), cosh(), tanh(),\n\
      e^() || exp() (exponent of Euler's number),\n\
      ln() (natural logarithm), log() (base 10 logarithm)\
"};

// Arguments to main are required for this to work as a console command
// taking a variable number of arguments.
int main(int argc, char* argv[]) {
  auto parsedArgs{ArgParser(helpStr)};
  parsedArgs.parse(argc, argv);
  if (parsedArgs.shouldExit()) return 0;
  Expression expression(parsedArgs.argString());
  std::cout << std::setprecision(parsedArgs.precision()) << expression.result()
            << std::endl;
  return 0;
}

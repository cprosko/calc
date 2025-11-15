#include "Expression.h"
#include <iostream>

// Arguments to main are required for this to work as a console command
// taking a variable number of arguments.
int main(int argc, char *argv[]) {
  std::cout << "Program name: " << argv[0] << std::endl;

  // Print other arguments and assemble expression
  std::string expression;
  for (int i{1}; i < argc; ++i) {
    std::cout << "Argument " << i << ": " << argv[i] << '\n';
    expression += argv[i];
  }

  // Test class initialization
  Expression myExpression(expression);
  std::cout << "Input expression: " << myExpression.expressionStr() << '\n';
  return 0;
}

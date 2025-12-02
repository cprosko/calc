#include "Expression.h"
#include <iostream>

int main() {
  std::cout << "Running debug main()." << '\n';
  std::vector<Expression> expressions{
      Expression("(2+3)*4"),
  };
  for (Expression expr : expressions) {
    expr.printCalculation();
  }
  for (Expression expr : expressions) {
    expr.result();
    std::cout << "RESULT: " << expr.result() << " for expression "
              << expr.expression() << "\n\n";
  }
}

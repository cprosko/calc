#include "Expression.h"
#include <iostream>

int main() {
  std::cout << "Running debug main()." << '\n';
  std::vector<Expression> expressions{
      // Check basic operations
      Expression("1.2 3"), Expression("2.0*3"), Expression("2.0^  3.0"),
      // Check BEDMAS
      Expression("5x3+2"), Expression("5+3x2"), Expression("12.3-4.1+1-12.1"),
      Expression("(3+5)x(4-2)"),
      // Check functions
      Expression("sin(3.14159/2)"),
      // Check more complicated expressions
      Expression("ln(2)xsin(3.14159/2)^3.0"),
      Expression("-1.0*ln((4.5+3)^4)-12"), Expression("(((4+2)))"),
      Expression("(log((4+5)^2+4^2)-3^2)")};
  for (Expression expr : expressions) {
    expr.result();
    std::cout << "RESULT: " << expr.result() << " for expression "
              << expr.expression() << "\n\n";
  }
}

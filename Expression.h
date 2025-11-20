#pragma once

// Standard library
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Calculator; // Forward declaration

class Expression {
public:
  // Enums
  enum class Operator {
    None,
    Plus,
    Minus,
    Times,
    Divide,
    Pow,
    Exp,
    Sqrt,
    Ln,
    Log,
    Sin,
    Cos,
    Tan,
    Sinh,
    Cosh,
    Tanh,
  };
  // Structs
  struct TokenizedExpression {
    std::vector<Expression> tokens;
    Operator                 function;
    std::vector<Operator>    binOps;
  };

  // Ensure default constructor exists even though we've defined others
  Expression()
      : expression_(), trimmedExpression_(), is_parsed_(false),
        is_calculated_(false), is_validated_(false), is_atomic_(false),
        show_calculation_(false), is_subexpression_(false), result_(0.0),
        operator_(), operands_(std::vector<Expression>()) {}
  explicit Expression(const std::string &expression,
                      bool is_subexpression = false)
      : expression_(expression), trimmedExpression_(), is_parsed_(false),
        is_calculated_(false), is_validated_(false), is_atomic_(false),
        show_calculation_(false), is_subexpression_(is_subexpression),
        result_(0.0), operator_(), operands_(std::vector<Expression>()) {
    parse_();
  }

  // Static functions
  static double calculate(const std::string &numOperator,
                          const double &operand) {
    return calculate_(operators_.at(numOperator), operand);
  }
  static double calculate(const std::string &numOperator,
                          const double &leftOperand,
                          const double &rightOperand) {
    return calculate_(operators_.at(numOperator), leftOperand, rightOperand);
  }

  // Public methods
  static double add(const double &a, const double &b) { return a + b; }
  static double subtract(const double &a, const double &b) { return a - b; }
  static double multiply(const double &a, const double &b) { return a * b; }
  static double divide(const double &a, const double &b) { return a / b; }

  const std::string expressionStr() { return expression_; }
  void set_expression(const std::string &expression);
  double result();
  double calculate() { return result(); }
  double calculate(const std::string &expression);
  void print_calculation();
  bool is_atomic();

private:
  // Private constants
  static const std::unordered_map<std::string_view, Operator> operators_;
  static const std::regex exprPattern_;
  static const std::regex numToken_;
  static const std::regex funcToken_;

  // Private methods
  static const std::string escapeRegex(std::string_view str);
  static double parsedNumber_(const std::string &numStr);
  void validate_(const std::string &expression);
  void parse_();
  static const TokenizedExpression tokenizedExpression_(const std::string &expression);
  static const int closingBracketIndex_(const std::string &str);
  double calculate_(const Operator &numOperator,
                    std::vector<Expression> &operands);
  static double calculate_(const Operator &numOperator, Expression &operand) {
    return calculate_(numOperator, operand.result());
  }
  static double calculate_(const Operator &numOperator, Expression &leftOperand,
                    Expression &rightOperand) {
    return calculate_(numOperator, leftOperand.result(), rightOperand.result());
  }
  static double calculate_(const Operator &numOperator, const double &operand);
  static double calculate_(const Operator &numOperator,
                           const double &leftOperand,
                           const double &rightOperand);
  static void checkNaN_(double num) {
    if (std::isnan(num))
      std::cout << "Warning: num is NaN." << '\n';
  }
  static const std::regex constructExprPattern_();

  // Private variables
  std::string expression_;
  std::string trimmedExpression_;
  bool is_parsed_;
  bool is_calculated_;
  bool is_validated_;
  bool is_atomic_;
  bool show_calculation_;
  bool is_subexpression_;
  double result_;
  Operator operator_;
  std::vector<Expression> operands_;
};

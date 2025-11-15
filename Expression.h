#pragma once

// Standard library
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

class Calculator; // Forward declaration

class Expression {
public:
  // Enums
  enum class Operator {
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
  struct OperatorMap {
    std::string_view key;
    Operator value;
  };

  // Ensure default constructor exists even though we've defined others
  Expression()
      : expression_(), is_parsed_(false), is_calculated_(false),
        is_validated_(false), is_atomic_(false), result_(0.0), operator_(),
        operands_(std::vector<Expression>()) {}
  explicit Expression(const std::string &expression)
      : expression_(expression), is_parsed_(false), is_calculated_(false),
        is_validated_(false), is_atomic_(false), result_(0.0), operator_(),
        operands_(std::vector<Expression>()) {
    parse_();
  }

  // Static functions
  static double calculate(const std::string &numOperator,
                          const double &operand) {
    return calculate_(parsedOperator_(numOperator), operand);
  }
  static double calculate(const std::string &numOperator,
                          const double &leftOperand,
                          const double &rightOperand) {
    return calculate_(parsedOperator_(numOperator), leftOperand, rightOperand);
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
  static constexpr std::array operators = {
      OperatorMap{"+", Operator::Plus},    OperatorMap{"-", Operator::Minus},
      OperatorMap{"*", Operator::Times},   OperatorMap{"x", Operator::Times},
      OperatorMap{"/", Operator::Divide},  OperatorMap{"^", Operator::Pow},
      OperatorMap{"e^", Operator::Exp},    OperatorMap{"exp", Operator::Exp},
      OperatorMap{"sqrt", Operator::Sqrt}, OperatorMap{"ln", Operator::Ln},
      OperatorMap{"log", Operator::Log},   OperatorMap{"sin", Operator::Sin},
      OperatorMap{"cos", Operator::Cos},   OperatorMap{"tan", Operator::Tan},
      OperatorMap{"sinh", Operator::Sinh}, OperatorMap{"cosh", Operator::Cosh},
      OperatorMap{"tanh", Operator::Tanh},
  };
  static const std::regex validExprPattern_;
  static const std::regex validNumberPattern_;

  // Private methods
  static const std::string escapeRegex(std::string_view str);
  static constexpr Operator
  parsedOperator_(const std::string_view &operatorStr) {
    for (auto &&opStr : operators)
      if (opStr.key == operatorStr)
        return opStr.value;
    throw std::runtime_error("No operator found matching " +
                             std::string(operatorStr) + ".");
  }
  static double parsedNumber_(const std::string &numStr);
  void validate_(const std::string &expression);
  void parse_();
  double calculate_(const Operator &numOperator,
                    std::vector<Expression> operands);
  double calculate_(const Operator &numOperator, Expression &operand) {
    return calculate_(numOperator, operand.result());
  }
  double calculate_(const Operator &numOperator, Expression &leftOperand,
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
  static const std::regex constructValidExprPattern_();

  // Private variables
  std::string expression_;
  bool is_parsed_;
  bool is_calculated_;
  bool is_validated_;
  bool is_atomic_;
  double result_;
  Operator operator_;
  std::vector<Expression> operands_;
};

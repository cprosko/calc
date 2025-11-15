// Internal headers
#include "Expression.h"

// Standard library
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <string>

void Expression::set_expression(const std::string &expression) {
  is_parsed_ = false;
  is_calculated_ = false;
  is_validated_ = false;
  validate_(expression);
  expression_ = expression;
}

double Expression::calculate(const std::string &expression) {
  set_expression(expression);
  return result();
}

double Expression::result() {
  if (is_calculated_) {
    return result_;
  }
  if (!is_parsed_) {
    parse_();
  }
  if (is_atomic_) {
    return result_; // Value has been calculated inside parse_()
  }
  result_ = std::numeric_limits<double>::quiet_NaN();
  result_ = calculate_(operator_, operands_);
  checkNaN_(result_);
  return result_;
}

void Expression::print_calculation() {
  // TODO
  std::cout << "TODO" << '\n';
}

bool Expression::is_atomic() {
  if (!is_parsed_) {
    parse_();
  }
  return is_atomic_;
}

const std::string Expression::escapeRegex(std::string_view str) {
  const std::string_view regexSpecials = R"(.^$*+?()[]{}|\)";
  std::string escaped;
  escaped.reserve(str.size() * 2);
  for (char c : str) {
    if (regexSpecials.find(c) != std::string::npos) {
      escaped += '\\';
    }
    escaped += c;
  }
  return escaped;
}

double Expression::parsedNumber_(const std::string &numStr) {
  double num;
  auto numMinuses(std::count(numStr.begin(), numStr.end(), '-'));
  int multiplier{numMinuses % 2 == 0 ? +1 : -1};
  num = multiplier;
  return num;
}

void Expression::validate_(const std::string &expression) {
  // TODO
  // Check parentheses are matched
  auto leftBrackets{std::count(expression.begin(), expression.end(), '(')};
  if (leftBrackets != std::count(expression.begin(), expression.end(), ')'))
    throw std::runtime_error("Unmatched parentheses in expression.");
  // Trim whitespace
  std::string trimmedExpression{expression};
  std::erase_if(trimmedExpression,
                [](unsigned char c) { return std::isspace(c); });
  // Check operators are all valid
  if (!std::regex_match(expression, validExprPattern_)) {
    throw std::runtime_error(
        "Invalid operators or numbers present in expression.");
  }
  return;
}

void Expression::parse_() {
  if (!is_validated_) {
    validate_(expression_);
  }
  if (std::regex_match(expression_, validNumberPattern_)) {
    is_atomic_ = true;
    result_ = parsedNumber_(expression_);
    is_calculated_ = true;
    return;
  }
  // TODO parse non-atomic expressions
  return;
}

double Expression::calculate_(const Operator &numOperator,
                              const double &operand) {
  double value{std::numeric_limits<double>::quiet_NaN()};
  switch (numOperator) {
  case Operator::Exp:
    value = std::exp(operand);
    break;
  case Operator::Sqrt:
    value = std::sqrt(operand);
    break;
  case Operator::Ln:
    value = std::log(operand);
    break;
  case Operator::Log:
    value = std::log10(operand);
    break;
  case Operator::Sin:
    value = std::sin(operand);
    break;
  case Operator::Cos:
    value = std::cos(operand);
    break;
  case Operator::Tan:
    value = std::tan(operand);
    break;
  case Operator::Sinh:
    value = std::sinh(operand);
    break;
  case Operator::Cosh:
    value = std::cosh(operand);
    break;
  case Operator::Tanh:
    value = std::tanh(operand);
    break;
  default:
    throw std::runtime_error("Invalid operator given single operand.");
  }
  checkNaN_(value);
  return value;
}

double Expression::calculate_(const Operator &numOperator,
                              const double &leftOperand,
                              const double &rightOperand) {
  double value{std::numeric_limits<double>::quiet_NaN()};
  switch (numOperator) {
  case Operator::Plus:
    value = leftOperand + rightOperand;
    break;
  case Operator::Minus:
    value = leftOperand - rightOperand;
    break;
  case Operator::Times:
    value = leftOperand * rightOperand;
    break;
  case Operator::Divide:
    value = leftOperand / rightOperand;
    break;
  case Operator::Pow:
    value = std::pow(leftOperand, rightOperand);
    break;
  default:
    throw std::runtime_error("Invalid operator given two operands.");
  }
  checkNaN_(value);
  return value;
}

double Expression::calculate_(const Operator &numOperator,
                              std::vector<Expression> operands) {
  switch (operands.size()) {
  case 1:
    result_ = calculate_(numOperator, operands[0]);
    break;
  case 2:
    result_ = calculate_(numOperator, operands[0], operands[1]);
    break;
  default:
    throw std::runtime_error(
        "Error: Expression has an invalid number of operands (" +
        std::to_string(operands.size()) + ")");
  }
  return result_;
}

const std::regex Expression::constructValidExprPattern_() {
  std::string pattern;
  bool first{true};
  for (OperatorMap op : operators) {
    if (!first)
      pattern += '|';
    pattern += escapeRegex(op.key);
    first = false;
  }
  pattern = "^(" + pattern + "|-|\\+|/|\\^|\\*|\\)|\\d+\\.?\\d*)+$";
  // TODO: remove this debug code
  std::cout << "validExprPattern: " << pattern << '\n';
  return std::regex(pattern);
}

const std::regex Expression::validExprPattern_{constructValidExprPattern_()};
const std::regex Expression::validNumberPattern_{
    std::regex(R"(-?\(?-?+?\d+\.?\d+\)?)")};

#ifdef EXPRESSION_DEBUG
int main() {
  std::cout << "Running debug main()." << '\n';
  return 0;
}
#endif

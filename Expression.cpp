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

// Namespaces
using namespace std::string_literals;

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
#ifndef EXPRESSION_DEBUG
  std::cout << "Operator: " << static_cast<int>(operator_) << '\n';
#endif
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
  std::string cleaned;
  // Remove any parentheses and whitespace from the string
  cleaned.reserve(numStr.size());
  for (char c : numStr) {
    if (c == '(' || c == ')' || std::isspace(static_cast<unsigned char>(c)))
      continue;
    cleaned += c;
  }
  // Convert from std::string to double
  return std::stod(cleaned);
}

void Expression::validate_(const std::string &expression) {
  // TODO: remove following debug code
#ifdef EXPRESSION_DEBUG
  std::cout << "Validating expression " << expression << '\n';
#endif
  // Check parentheses are matched
  auto leftBrackets{std::count(expression.begin(), expression.end(), '(')};
  if (leftBrackets != std::count(expression.begin(), expression.end(), ')'))
    throw std::runtime_error("Unmatched parentheses in expression.");
  // Trim whitespace
  std::string trimmedExpression{expression};
  std::erase_if(trimmedExpression,
                [](unsigned char c) { return std::isspace(c); });
#ifdef EXPRESSION_DEBUG
  std::cout << "Expression after whitespace trimming: " << trimmedExpression
            << '\n';
#endif
  // Remove unnecessary outer parentheses
  if (trimmedExpression.front() == '(' && trimmedExpression.back() == ')') {
    trimmedExpression.erase(trimmedExpression.begin());
    trimmedExpression.pop_back();
  }
#ifdef EXPRESSION_DEBUG
  std::cout << "Expression after parenthesis removal: " << trimmedExpression
            << '\n';
#endif
  // Remove leading '+' sign
  if (trimmedExpression.front() == '+')
    trimmedExpression.erase(trimmedExpression.begin());
#ifdef EXPRESSION_DEBUG
  std::cout << "Expression after '+' removal: " << trimmedExpression << '\n';
#endif
  // Check operators are all valid
  if (!std::regex_match(trimmedExpression, exprPattern_)) {
    throw std::runtime_error(
        "Invalid operators or numbers present in expression: " +
        trimmedExpression);
  }
  trimmedExpression_ = trimmedExpression;
  return;
}

void Expression::parse_() {
  if (!is_validated_) {
    validate_(expression_);
  }
  // TODO: remove debug code
#ifdef EXPRESSION_DEBUG
  std::cout << "valid!\n";
#endif
  operands_.clear();
  // Check if expression is just a number
  if (!is_atomic_ && std::regex_match(trimmedExpression_, numberPattern_)) {
    // TODO: remove debug code
#ifdef EXPRESSION_DEBUG
    std::cout << "atomic!\n";
#endif
    is_atomic_ = true;
    result_ = parsedNumber_(trimmedExpression_);
    is_calculated_ = true;
    return;
  }
  // Break expression down into subexpressions
  // based on 'BEDMAS' arithmetic rules
  // Outer parentheses were removed in trimmedExpression_ during validation

  // Checking for -1 multiplier only requires looking at first character, so do
  // this first
  if (trimmedExpression_.front() == '-') {
#ifdef EXPRESSION_DEBUG
    std::cout << "Expression type: -1 multiplier\n";
#endif
    operands_.push_back(Expression("-1"));
    operator_ = Operator::Times;
    operands_.push_back(Expression(trimmedExpression_.substr(1)));
    return;
  }

  std::smatch match; // initialized on below line
  if (!std::regex_match(trimmedExpression_, match, operandPattern_)) {
    throw std::runtime_error("Invalid syntax in expression " + expression_);
  }
  if (match[7].matched) {
    // First block including exponent is only one operand in this expression
    char firstChar{match[7].str().front()};
    operands_.push_back(Expression(match[1].str() + match[5].str()));
    if (std::find(binOperators_.begin(), binOperators_.end(), firstChar) !=
        binOperators_.end()) {
#ifdef EXPRESSION_DEBUG
      std::cout << "Expression type: a +|-|x|/|*|^ b\n";
#endif
      // Next character is a binary operator (e.g. +, -, x, /, *, ^)
      std::string_view charStr(&firstChar, 1);
      operator_ = operators_.at(charStr);
      operands_.push_back(Expression(match[7].str().substr(1)));
    } else {
#ifdef EXPRESSION_DEBUG
      std::cout << "Expression type: (a)(b)\n";
#endif
      // Next part is just a factor to multiply operands_[0] by
      operator_ = Operator::Times;
      operands_.push_back(Expression(match[7].str()));
    }
    return;
  }
  // match[1] and match[5] are entire expression, so need to be broken down
  if (match[5].matched) {
    // Expression is of form match[1]^match[6|7]
    std::string exponent{match[5].str()};
    if (match[6].matched)
      exponent = match[6].str();
    if (match[7].matched)
      exponent = match[6].str();
    if (match[1].str() == "e") {
#ifdef EXPRESSION_DEBUG
      std::cout << "Expression type: e^a\n";
#endif
      operator_ = Operator::Exp;
      operands_.push_back(Expression(match[6].str())); // exponent contents
    } else {
#ifdef EXPRESSION_DEBUG
      std::cout << "Expression type: a^b\n";
#endif
      operator_ = Operator::Pow;
      operands_.push_back(Expression(match[1].str())); // base contents
      operands_.push_back(Expression(match[6].str())); // exponent contents
    }
    return;
  }
  // No exponent in base expression, need to break down match[1]
  // Because !match[5].matched, this must be a math function
  std::smatch funcMatch;
  if (!std::regex_match(match[1].first, match[1].second, funcMatch,
                        funcPattern_))
    throw std::runtime_error("Expected function at beginning of expression but "
                             "none was found: check syntax.");
#ifdef EXPRESSION_DEBUG
  std::cout << "Expression type: func(a)\n";
#endif
  operator_ = operators_.at(funcMatch[1].str());
  operands_.push_back(Expression(funcMatch[2].str()));
  // TODO: this doesn't account for BEDMAS: e.g. 4+3*2
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

const std::regex Expression::constructExprPattern_() {
  std::string pattern;
  bool first{true};
  for (const auto &[key, value] : operators_) {
    if (!first)
      pattern += '|';
    pattern += escapeRegex(key);
    first = false;
  }
  pattern = R"(^()"s + pattern + R"(|\(|\)|\d+\.?\d*)+$)";
  // TODO: remove this debug code
#ifdef EXPRESSION_DEBUG
  std::cout << "exprPattern: " << pattern << '\n';
#endif
  return std::regex(pattern);
}

const std::regex Expression::constructOperandPattern_() {
  std::string pattern{
      // base block (group 1), inner brackets (opt., group 2, 3, 4)
      R"(^(\((.*?)\)|[a-z]+(\(.*?\)|(\d+\.?\d*))|\d+\.?\d*|e))"s
      // check for exponent (opt., group 5), inner brackets (opt., group 6, 7)
      + R"((\^(\(.*?\)|[a-z]+(\(.*?\))|\d+\.?\d*))?)"
      // Remaining operators and blocks (opt., group 7)
      + R"((.+?)?$)"};
  // TODO: remove this debug code
#ifdef EXPRESSION_DEBUG
  std::cout << "operandPattern: " << pattern << '\n';
#endif
  return std::regex(pattern);
}

const std::unordered_map<std::string_view, Expression::Operator>
    Expression::operators_{{"+", Expression::Operator::Plus},
                           {"-", Expression::Operator::Minus},
                           {"*", Expression::Operator::Times},
                           {"x", Expression::Operator::Times},
                           {"/", Expression::Operator::Divide},
                           {"^", Expression::Operator::Pow},
                           {"e^", Expression::Operator::Exp},
                           {"exp", Expression::Operator::Exp},
                           {"sqrt", Expression::Operator::Sqrt},
                           {"ln", Expression::Operator::Ln},
                           {"log", Expression::Operator::Log},
                           {"sin", Expression::Operator::Sin},
                           {"cos", Expression::Operator::Cos},
                           {"tan", Expression::Operator::Tan},
                           {"sinh", Expression::Operator::Sinh},
                           {"cosh", Expression::Operator::Cosh},
                           {"tanh", Expression::Operator::Tanh}};
const std::regex Expression::exprPattern_{constructExprPattern_()};
const std::regex Expression::numberPattern_{std::regex(R"(\d+\.?\d*)")};
const std::regex Expression::operandPattern_{constructOperandPattern_()};
const std::regex Expression::funcPattern_{std::regex(R"(([a-z]+)\(?(.*?)\)?)")};

#ifdef EXPRESSION_DEBUG
int main() {
  std::cout << "Running debug main()." << '\n';
  std::vector<Expression> expressions{// Check basic operations
                                      Expression("1.23"), Expression("2.0*3"),
                                      Expression("2.0^3.0"),
                                      // Check BEDMAS
                                      Expression("5x3+2"), Expression("5+3x2"),
                                      // Check functions
                                      Expression("sin(3.14159/2)"),
                                      // Check more complicated expressions
                                      Expression("ln(2)xsin(3.14159/2)^3.0")};
  for (Expression expr : expressions) {
    std::cout << "Result:\n" << expr.result() << "\n\n";
  }
}
#endif

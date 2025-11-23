// Internal headers
#include "Expression.h"

// Standard library
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>

// Namespaces
using namespace std::string_literals;

// ----------------------------------------------------------------------------
// Public Methods
// ----------------------------------------------------------------------------
std::string Expression::expression() {
  if (expression_.size() > 0) {
    return expression_;
  }
  if (tokens_.tokens.size() == 0) {
    if (isCalculated_) {
      std::ostringstream oss;
      oss << std::setprecision(precision_) << result();
      expression_ = oss.str();
      trimmedExpression_ = expression_;
      return expression_;
    }
    return "<NO EXPRESSION>";
  }
  if (tokens_.tokens.size() == 1) {
    expression_ = tokens_.tokens[0].expression();
    if (tokens_.function != Operator::None) {
      expression_ =
          static_cast<std::string>(operatorStrings_.at(tokens_.function)) +
          "("s + expression_ + ")"s;
    }
    trimmedExpression_ = expression_;
    return expression_;
  }
  expression_ = "";
  for (size_t i{0}; i < tokens_.binOps.size(); ++i) {
    expression_ +=
        tokens_.tokens[i].expression() +
        static_cast<std::string>(operatorStrings_.at(tokens_.binOps[i]));
  }
  expression_ += tokens_.tokens.back().expression();
  trimmedExpression_ = expression_;
  return expression_;
}

void Expression::set_expression(const std::string &expression) {
  isParsed_ = false;
  isCalculated_ = false;
  isValidated_ = false;
  validate_(expression);
  expression_ = expression;
}

double Expression::calculate(const std::string &expression) {
  set_expression(expression);
  return result();
}

double Expression::result() {
  if (isCalculated_) {
    return result_;
  }
  if (isTokenized_) {
    outerStep_ = lastCalculationStep_(tokens_);
  } else if (!isParsed_) {
    parse_();
  }
  if (isAtomic_) {
    return result_; // Value has been calculated inside parse_()
  }
  result_ = std::numeric_limits<double>::quiet_NaN();
  result_ = calculate_(outerStep_);
  isCalculated_ = true;
  checkNaN_(result_);
  return result_;
}

void Expression::printCalculation() {
  showCalculation_ = true;
  isCalculated_ = false;
  parse_();
  std::cout << "Result: " << result_ << '\n';
  showCalculation_ = false;
}

bool Expression::isAtomic() {
  if (!isParsed_) {
    parse_();
  }
  return isAtomic_;
}

// ----------------------------------------------------------------------------
// Private Methods
// ----------------------------------------------------------------------------
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
  // Check parentheses are matched
  auto leftBrackets{std::count(expression.begin(), expression.end(), '(')};
  if (leftBrackets != std::count(expression.begin(), expression.end(), ')'))
    throw std::runtime_error("Unmatched parentheses in expression.");
  // Trim whitespace
  std::string trimmedExpression{expression};
  std::erase_if(trimmedExpression,
                [](unsigned char c) { return std::isspace(c); });
  // Remove unnecessary outer parentheses
  while (trimmedExpression.front() == '(' &&
         closingBracketIndex_(trimmedExpression) + 1 ==
             trimmedExpression.size()) {
    trimmedExpression.erase(trimmedExpression.begin());
    trimmedExpression.pop_back();
  }
  // Remove leading '+' sign
  if (trimmedExpression.front() == '+')
    trimmedExpression.erase(trimmedExpression.begin());
  // Check operators are all valid
  if (!std::regex_match(trimmedExpression, exprPattern_)) {
    throw std::runtime_error(
        "Invalid operators or numbers present in expression: " +
        trimmedExpression);
  }
  trimmedExpression_ = trimmedExpression;
  return;
}

// Arrange expression into subexpressions
// Arranged such that outermost expression consists of the last operation to
// calculate on subexpressions according to BEDMAS, so full result can be found
// recursively.
void Expression::parse_() {
  // TODO: Add printing statements when showCalculation_ == true
  if (!isValidated_) {
    validate_(expression_);
  }
  // Clear previous results
  tokens_.tokens.clear();
  tokens_.binOps.clear();
  outerStep_.operands.clear();
  outerStep_.operators.clear();
  // Check if expression is just a number
  if (!isAtomic_ && std::regex_match(trimmedExpression_, numPattern_)) {
    isAtomic_ = true;
  }
  if (isAtomic_) {
    result_ = parsedNumber_(trimmedExpression_);
    isCalculated_ = true;
    return;
  }
  // Break expression down into recursive subexpressions based on BEDMAS
  // arithmetic rules
  // Outer parentheses were removed in trimmedExpression_ during validation
  tokens_ = tokenizedExpression_(trimmedExpression_);
  outerStep_ = lastCalculationStep_(tokens_);
}

Expression::TokenizedExpression
Expression::tokenizedExpression_(const std::string &expression) {
  std::vector<Expression> subexpressions;
  Operator function{Operator::None};     // on whole expression
  std::vector<Operator> binaryOperators; // between subexpressions
  std::string remainingExpression;
  // Check for leading operators
  switch (expression.front()) {
  case '-':
    subexpressions.push_back(Expression(-1.0));
    binaryOperators.push_back(Operator::Times);
    remainingExpression = expression.substr(1);
    break;
  case '+':
    remainingExpression = expression.substr(1);
    break;
  case 'x':
  case '/':
  case '*':
  case '^':
  case '%':
    throw std::runtime_error("Leading binary operator found in expression.");
  default:
    remainingExpression = expression;
  }

  // Tokenize the string from left to right into subexpressions and operators
  bool prevTokenWasBinOp{false};
  std::smatch match;
  while (remainingExpression.size() > 0) {
    size_t closingIndex;    // End index of token
    bool foundMatch{false}; // Used to avoid regex matching when unnecessary
    char frontChar{remainingExpression.front()};
    switch (frontChar) {
    case '+':
    case '-':
    case 'x':
    case '*':
    case '/':
    case '^':
    case '%':
      binaryOperators.push_back(operators_.at(std::string(1, frontChar)));
      prevTokenWasBinOp = true;
      remainingExpression = remainingExpression.substr(1);
      continue;
    case '(':
      closingIndex = closingBracketIndex_(remainingExpression);
      subexpressions.push_back(
          Expression(remainingExpression.substr(1, closingIndex - 1)));
      remainingExpression = remainingExpression.substr(closingIndex + 1);
      prevTokenWasBinOp = false;
      foundMatch = true;
    }
    // Is remaining expression wrapped in brackets?
    if (!foundMatch &&
        std::regex_match(remainingExpression, match, numToken_)) {
      // Construct expression with result already stored, since it's just a
      // double
      subexpressions.push_back(Expression(std::stod(match[1].str())));
      closingIndex = static_cast<size_t>(match[1].length() - 1);
      remainingExpression = match[2].str();
      prevTokenWasBinOp = false;
    } else if (!foundMatch &&
               std::regex_match(remainingExpression, match, funcToken_)) {
      if (!match[2].matched)
        throw std::runtime_error("Function in expression without argument.");
      closingIndex = closingBracketIndex_(match[2].str(), true);
      std::string argument{match[2].str().substr(0, closingIndex)};
      // Account for size of function and opening bracket

      closingIndex += static_cast<size_t>(match[1].length() + 1);
      if (closingIndex + 1 == remainingExpression.size()) {
        // expression is just function call on inner expression
        function = operators_.at(match[1].str());
        subexpressions.push_back(Expression(argument));
        remainingExpression.clear();
      } else {
        subexpressions.push_back(
            Expression(remainingExpression.substr(0, closingIndex + 1)));
        remainingExpression = remainingExpression.substr(closingIndex + 1);
        prevTokenWasBinOp = false;
      }
    } else if (!foundMatch) {
      throw std::runtime_error("Unexpected token found during tokenization.");
    }
    if (prevTokenWasBinOp)
      binaryOperators.push_back(Operator::Times);
    foundMatch = false;
  }
  TokenizedExpression result = {.tokens = subexpressions,
                                .binOps = binaryOperators,
                                .function = function};
  return result;
}

Expression::Step Expression::lastCalculationStep_(TokenizedExpression &tokens) {
  Step lastStep;
  // Assumes that any functions are present, they wrap the whole expression
  if (tokens.tokens.size() == 1) {
    if (!tokens.binOps.empty() || tokens.function == Operator::None)
      throw std::runtime_error(
          "Found binary operator acting on single token and no unary operator");
    lastStep.operators.push_back(tokens.function);
    lastStep.operands.push_back(tokens.tokens[0]);
    return lastStep;
  }
  if (tokens.binOps.size() + 1 != tokens.tokens.size())
    throw std::runtime_error(
        "Too many or too few binary operators for number of tokens.");
  if (tokens.tokens.size() == 2) {
    lastStep.operators.push_back(tokens.binOps[0]);
    lastStep.operands.push_back(tokens.tokens[0]);
    lastStep.operands.push_back(tokens.tokens[1]);
    return lastStep;
  }
  // From lowest to highest priority in BEDMAS:
  // priority 3 for + -, 2 for * x /, 1 for ^
  int priority{3};
  int numSteps{0};
  bool foundStepThisIter{false};
  size_t operInd{0};
  size_t lastUngroupedTokenInd{0};
  while (priority > 0 && (numSteps == 0 || operInd != 0)) {
    Operator op{tokens.binOps[operInd]};
    if ((priority == 3 && (op == Operator::Plus || op == Operator::Minus)) ||
        (priority == 2 && (op == Operator::Times || op == Operator::Divide ||
                           op == Operator::Mod)) ||
        priority == 1) {
      foundStepThisIter = true;
      numSteps++;
      lastStep.operators.push_back(op);
      if (operInd - lastUngroupedTokenInd == 0) {
        lastStep.operands.push_back(tokens.tokens[lastUngroupedTokenInd]);
      } else {
        lastStep.operands.push_back(
            combinedTokens_(tokens, lastUngroupedTokenInd, operInd + 1));
      }
      lastUngroupedTokenInd = operInd + 1;
    }
    if (operInd + 1 == tokens.binOps.size()) {
      if (foundStepThisIter) {
        // Must be only one token left
        lastStep.operands.push_back(tokens.tokens.back());
        break;
      }
      if (priority == 1 && !foundStepThisIter)
        throw std::runtime_error(
            "Failed to find lowest-priority calculation step.");
      operInd = 0;
      priority--;
    } else {
      operInd++;
    }
    foundStepThisIter = false;
  }
  if (numSteps > 0 && lastUngroupedTokenInd < tokens.binOps.size()) {
    // Have to append combined token of all remaining tokens
    lastStep.operands.push_back(
        combinedTokens_(tokens, lastUngroupedTokenInd, tokens.tokens.size()));
  }
  return lastStep;
}

Expression Expression::combinedTokens_(TokenizedExpression &tokens,
                                       const size_t &startInd,
                                       const size_t &stopInd) {
  if (stopInd - startInd < 1)
    throw std::runtime_error("Attempting to 'combine' zero tokens.");
  auto tokenSlice = tokens.tokens | std::views::drop(startInd) |
                    std::views::take(stopInd - startInd);
  auto operSlice = tokens.binOps | std::views::drop(startInd) |
                   std::views::take(stopInd - startInd - 1);
  TokenizedExpression subTokens{
      .tokens = std::vector<Expression>(tokenSlice.begin(), tokenSlice.end()),
      .binOps = std::vector<Operator>(operSlice.begin(), operSlice.end()),
  };
  return Expression(subTokens);
}

size_t Expression::closingBracketIndex_(const std::string &str,
                                        const bool includeFrontBracket) {
  // Find char index of ')' matching some '(' at front or left of string
  // Returns -1 if no match is found.
  int unclosedBrackets{(str.front() == '(' && !includeFrontBracket) ? 0 : 1};
  size_t charInd;
  for (charInd = 0; charInd < str.size(); ++charInd) {
    char c{str[charInd]};
    if (c == '(') {
      unclosedBrackets++;
      continue;
    }
    if (c == ')' && --unclosedBrackets == 0) {
      break;
    }
  }
  if (unclosedBrackets == 0)
    return charInd;
  throw std::runtime_error("No matching close bracket found.");
}

double Expression::calculate_(const Operator &numOperator,
                              const double operand) {
  double value{std::numeric_limits<double>::quiet_NaN()};
  switch (numOperator) {
  case Operator::None:
    value = operand;
    break;
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
                              const double leftOperand,
                              const double rightOperand) {
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
  case Operator::Mod:
    value = std::fmod(leftOperand, rightOperand);
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

double Expression::calculate_(Step &step) {
  // Apply operators to operands left-to-right
  // Assumes unary operators only appear when there is one operand
  if (step.operands.size() == 1) {
    if (step.operators.size() != 1)
      throw std::runtime_error("Too many operators relative to operands.");
    return calculate_(step.operators[0], step.operands[0].result());
  }
  if (step.operators.size() + 1 != step.operands.size()) {
    throw std::runtime_error(
        "For binary operations, there must be one less operator than operands");
  }
  double runningResult{step.operands[0].result()};
  for (size_t i{1}; i < step.operands.size(); ++i) {
    runningResult = calculate_(step.operators[i - 1], runningResult,
                               step.operands[i].result());
  }
  return runningResult;
}

const std::unordered_map<Expression::Operator, std::string_view>
Expression::constructOperatorStrings_() {
  std::unordered_map<Operator, std::string_view> map;
  for (const auto &[key, value] : operators_) {
    if (value == Operator::Times) {
      map[value] = "x";
      continue;
    }
    map[value] = key;
  }
  return map;
}

const std::regex Expression::constructExprPattern_() {
  std::string pattern;
  bool first{true};
  for (const auto &[key, value] : operators_) {
    if (!first)
      pattern += '|';
    pattern += escapeRegex_(key);
    first = false;
  }
  pattern = R"(^()"s + pattern + R"(|\(|\)|\d+\.?\d*)+$)";
  return std::regex(pattern);
}

const std::string Expression::escapeRegex_(std::string_view str) {
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

const std::unordered_map<std::string_view, Expression::Operator>
    Expression::operators_{{"+", Expression::Operator::Plus},
                           {"-", Expression::Operator::Minus},
                           {"*", Expression::Operator::Times},
                           {"x", Expression::Operator::Times},
                           {"/", Expression::Operator::Divide},
                           {"^", Expression::Operator::Pow},
                           {"%", Expression::Operator::Mod},
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
                           {"tanh", Expression::Operator::Tanh},
                           {"NULL", Expression::Operator::None}};
const std::unordered_map<Expression::Operator, std::string_view>
    Expression::operatorStrings_{constructOperatorStrings_()};
const std::regex Expression::exprPattern_{constructExprPattern_()};
const std::regex Expression::numPattern_{std::regex(R"(^\d+\.?\d*$)")};
const std::regex Expression::numToken_{std::regex(R"(^(\d+\.?\d*)(.*))")};
const std::regex Expression::funcToken_{std::regex(R"(^([a-z]+|e^)\((.+))")};


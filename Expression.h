#pragma once

// Standard library
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <regex>
#include <stdexcept>
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
    Mod,
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
    std::vector<Expression> tokens{};
    std::vector<Operator> binOps{};
    Operator function{Operator::None};
  };
  struct Step {
    std::vector<Operator> operators{};
    std::vector<Expression> operands{};
  };

  // Ensure default constructor exists even though we've defined others
  Expression()
      : expression_(), trimmedExpression_(), isValidated_(false),
        isParsed_(false), isTokenized_(false), isCalculated_(false),
        isAtomic_(false), showCalculation_(false), isSubexpression_(false),
        result_(0.0), tokens_(), outerStep_() {}
  explicit Expression(const std::string &expression,
                      bool isSubexpression = false,
                      bool showCalculation = false)
      : expression_(expression), trimmedExpression_(),
        isValidated_(isSubexpression), isParsed_(false), isTokenized_(false),
        isCalculated_(false), isAtomic_(false),
        showCalculation_(showCalculation), isSubexpression_(isSubexpression),
        result_(0.0), tokens_(), outerStep_() {}
  explicit Expression(TokenizedExpression &tokens, bool isSubexpression = true,
                      bool showCalculation = false)
      : expression_(), trimmedExpression_(), isValidated_(isSubexpression),
        isParsed_(false), isTokenized_(true), isCalculated_(false),
        isAtomic_(false), showCalculation_(showCalculation),
        isSubexpression_(isSubexpression), result_(0.0), tokens_(tokens),
        outerStep_() {
    if (tokens.tokens.size() == 1) {
      if (tokens.tokens[0].isCalculated_) {
        isCalculated_ = true;
        result_ = tokens.tokens[0].result_;
      } else {
        throw std::runtime_error(
            "Cannot initialize expressions using TokenizedExpression with only "
            "one uncalculated token.");
      }
    }
  }
  explicit Expression(double result, bool isSubexpression = true)
      : expression_(), trimmedExpression_(), isValidated_(true),
        isParsed_(true), isTokenized_(true), isCalculated_(true),
        isAtomic_(true), showCalculation_(false),
        isSubexpression_(isSubexpression), result_(result), tokens_(),
        outerStep_() {}

  // Public methods
  static double add(const double a, const double b) { return a + b; }
  static double subtract(const double a, const double b) { return a - b; }
  static double multiply(const double a, const double b) { return a * b; }
  static double divide(const double a, const double b) { return a / b; }

  const std::string expressionStr() { return expression_; }
  void set_expression(const std::string &expression);
  double result();
  double calculate() { return result(); }
  double calculate(const std::string &expression);
  void printCalculation();
  bool isAtomic();

private:
  // Private constants
  static const std::unordered_map<std::string_view, Operator> operators_;
  static const std::unordered_map<Operator, std::string_view> operatorStrings_;
  static const std::regex exprPattern_;
  static const std::regex numPattern_;
  static const std::regex numToken_;
  static const std::regex funcToken_;

  // Private methods
  static const std::string escapeRegex(std::string_view str);
  static double parsedNumber_(const std::string &numStr);
  void validate_(const std::string &expression);
  void parse_();
  static TokenizedExpression
  tokenizedExpression_(const std::string &expression);
  static Step lastCalculationStep_(TokenizedExpression &tokens);
  static Expression combinedTokens_(TokenizedExpression &tokens,
                                    const size_t &startInd,
                                    const size_t &stopInd);
  static size_t closingBracketIndex_(const std::string &str,
                                     const bool includeFrontBracket = false);
  static double calculate_(Step &step);
  static double calculate_(const Operator &oper, const double operand);
  static double calculate_(const Operator &oper, const double leftOperand,
                           const double rightOperand);
  static void checkNaN_(double num) {
    if (std::isnan(num))
      std::cout << "Warning: num is NaN." << '\n';
  }
  static const std::unordered_map<Operator, std::string_view>
  constructOperatorStrings_();
  static const std::regex constructExprPattern_();

  // Private variables
  std::string expression_;
  std::string trimmedExpression_;
  bool isValidated_;
  bool isParsed_;
  bool isTokenized_;
  bool isCalculated_;
  bool isAtomic_;
  bool showCalculation_;
  bool isSubexpression_;
  double result_;
  TokenizedExpression tokens_;
  Step outerStep_;
};

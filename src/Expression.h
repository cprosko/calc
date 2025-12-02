#pragma once

// Standard library
#include <cmath>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/******************************************************************************
 * Class for parsing strings of mathematical expressions and evaluating them.
 *
 * The class is initialized with some input defining the expression, typically
 * a string. The input is parsed as needed, ie. when result() is called to
 * evalute the expression. Operators supported in the expression include
 *   - parentheses '('/')'
 *   - basic mathematical operators: +, -, x or *, /, ^, and % (modulus)
 *   - some mathematical functions: sqrt(), sin(), cos(), tan(), sinh(), cosh(),
 *       tanh(), ln() (natural logarithm), log() (base 10 logarithm),
 *       and e^() (Exponent of Euler's number)
 * Any mathematically valid combination of these operators is valid as an input
 * string, nested or otherwise, but functions must be followed by their
 * arguments enclosed in parentheses. Note: whitespace is ignored in input
 * strings.
 *****************************************************************************/
class Expression {
public:
  // Enums
  enum class Operator {
    None, /// Represents the identity, or no operation.
    Plus,
    Minus,
    Times,
    Divide,
    Pow, /// One number to the power of another: x^y
    Mod,
    Exp, /// A power of Euler's number: e^()
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

  /// Subexpressions and their connecting operators composing an Expression
  struct TokenizedExpression {
    std::vector<Expression> tokens{};
    std::vector<Operator> binOps{};
    Operator function{Operator::None};
  };
  /// Represents a single mathematical calculation to be conducted
  struct Step {
    std::vector<Operator> operators{};
    std::vector<Expression> operands{};
  };

  // Ensure default constructor exists even though we've defined others
  Expression()
      : precision(3), expression_(), trimmedExpression_(), hasBrackets_(false),
        isValidated_(false), isParsed_(false), isTokenized_(false),
        isCalculated_(false), isAtomic_(false), showCalculation_(false),
        isSubexpression_(false), result_(0.0), tokens_(), outerStep_() {}
  explicit Expression(const std::string &expr, bool isSubexpression = false,
                      bool showCalculation = false, bool hasBrackets = false)
      : precision(3), expression_(expr),
        trimmedExpression_(isSubexpression ? expr : ""),
        hasBrackets_(hasBrackets), isValidated_(isSubexpression),
        isParsed_(false), isTokenized_(false), isCalculated_(false),
        isAtomic_(false), showCalculation_(showCalculation),
        isSubexpression_(isSubexpression), result_(0.0), tokens_(),
        outerStep_() {
    if (showCalculation)
      std::cout << "Expression instantiated: " << expression() << std::endl;
  }
  explicit Expression(TokenizedExpression &tokens, bool isSubexpression = true,
                      bool showCalculation = false, bool hasBrackets = false)
      : precision(3), expression_(), trimmedExpression_(),
        hasBrackets_(hasBrackets), isValidated_(isSubexpression),
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
    if (showCalculation)
      std::cout << "Expression instantiated: " << expression() << std::endl;
  }
  explicit Expression(double result, bool isSubexpression = true,
                      bool hasBrackets = false)
      : precision(3), expression_(), trimmedExpression_(),
        hasBrackets_(hasBrackets), isValidated_(true), isParsed_(true),
        isTokenized_(true), isCalculated_(true), isAtomic_(true),
        showCalculation_(false), isSubexpression_(isSubexpression),
        result_(result), tokens_(), outerStep_() {}

  // Public methods

  /// The string form of this mathematical Expression
  std::string expression();
  /// Reset the Expression with a new mathematical Expression
  void set_expression(const std::string &expression);
  /// Set a new expression and calculate the result()
  double calculate(const std::string &expression);
  /// Calculate or retrieve the result of the expression
  double result();
  /// TODO: Not implemented
  void printCalculation();
  /// Partially check whether a string is a valid mathematical expression
  void validate();
  /// Whether character is a binary operator like +,-,*,x,/,%
  static bool isBinaryOperator(const char c);
  /// Whether or not the expression is a number or nontrivial expression
  bool isAtomic();
  /// Whether or not input expression string was validated
  bool isValidated() { return isValidated_; }

  // Public variables
  /// Number of digits to show after decimal in scientific notation
  int precision;

private:
  // Private constants

  /// Correspondence between written operators and their Operator representation
  static const std::unordered_map<std::string_view, Operator> operators_;
  /// Correspondence between Operator objects and their string form
  static const std::unordered_map<Operator, std::string_view> operatorStrings_;
  /// RegEx pattern for validating a string as a mathematical expression
  static const std::regex exprPattern_;
  /// RegEx pattern matching an integer or real number
  static const std::regex numPattern_;
  /// RegEx pattern for matching a string beginning with a number
  static const std::regex numToken_;
  /// RegEx pattern for matching a string beginning with a function call
  static const std::regex funcToken_;

  // Private methods

  /// TODO: Not implemented
  void calculateNextStep_();
  /// TODO: Not implemented
  void printPartialCalculation_();
  /// Whether all immediate child expressions have been calculated
  bool subexpressionsCalculated_();
  /// Translates a number in string form into a double for calculation
  static double parsedNumber_(const std::string &numStr);
  /// Parse the expression into tokens and determine the first calculation step
  void parse_();
  /// Parse an expression into token Expressions and operators for calculation
  void tokenizeExpression_(bool showCalculation = false);
  /// Determine the last calculation step according to BEDMAS, for recursive
  /// evaluation
  static Step lastCalculationStep_(TokenizedExpression &tokens);
  /// Merge tokens and operations back into a single Expression
  static Expression combinedTokens_(TokenizedExpression &tokens,
                                    const size_t &startInd,
                                    const size_t &stopInd,
                                    bool showCalculation = false);
  /// Find the ')' parenthesis character index matching a beginning '('
  static size_t closingBracketIndex_(const std::string &str,
                                     const bool includeFrontBracket = false);
  /// Calculate a mathematical operation bundled as a Step object
  static double calculate_(Step &step);
  /// Calculate a unary mathematical Operator on a number
  static double calculate_(const Operator &oper, const double operand);
  /// Calculate a binary mathematical Operator acting on two numbers
  static double calculate_(const Operator &oper, const double leftOperand,
                           const double rightOperand);
  /// Warn if an input number is not a number
  static void checkNaN_(double num) {
    if (std::isnan(num))
      std::cout << "Warning: num is NaN." << '\n';
  }
  /// Generate the Operator-string map
  static const std::unordered_map<Operator, std::string_view>
  constructOperatorStrings_();
  /// Return a string with all special RegEx characters escaped
  static const std::string escapeRegex_(std::string_view str);
  /// Construct the RegEx pattern for validating expressions
  static const std::regex constructExprPattern_();

  // Private variables

  /// String form of this Expression, potentially as raw input
  std::string expression_;
  /// String form of this Expression with whitespace and outer brackets removed
  std::string trimmedExpression_;
  bool hasBrackets_;     /// Whether (sub)expression is wrapped in parentheses
  bool isValidated_;     /// Whether the Expression string has been validated
  bool isParsed_;        /// Whether the Expression string has been parsed fully
  bool isTokenized_;     /// Whether the Expression string has been tokenized
  bool isCalculated_;    /// Whether the result of the Expression is calculated
  bool isAtomic_;        /// Whether the expression is just a number or compound
  bool showCalculation_; /// Whether to show verbose output of calculations
  bool isSubexpression_; /// Whether Expression is subexpression to a parent
  double result_;        /// Result of the mathematical expression
  /// Expression broken down into sub-Expressions and Operators
  TokenizedExpression tokens_;
  /// Last calculation step to perform on Expression re: BEDMAS
  Step outerStep_;
};

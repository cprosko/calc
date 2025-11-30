#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ArgParser.h"
#include "Expression.h"

#define TOLERANCE 1e-7

bool nearEqual(double a, double b, double epsilon = TOLERANCE) {
  double diff{std::fabs(a - b)};
  // Check if absolute difference is small enough
  if (diff <= epsilon) {
    return true;
  }
  // Otherwise check relative difference
  double largestAbs{std::max(std::fabs(a), std::fabs(b))};
  return diff <= largestAbs * epsilon;
}

void resetOstringstream(std::ostringstream &oss) {
  oss.str("");
  oss.clear();
}

const std::vector<std::pair<std::string, double>> basicExprResults = {
    {{"123.456", 123.456},
     {"1 + 1", 2.0},
     {"2 x 3 + 4", 10.0},
     {"2 + 3 x 4", 14.0},
     {"(2 + 3) x 4", 20.0},
     {"((2 + 3)^2 - 2)", 23.0},
     {"ln(1)", 0.0},
     {"sin(3.141592654 / 2.0)", 1.0},
     {"-1xcos(0.0)", -1.0},
     {"-cos(0.0)", -1.0}}};

const std::vector<std::pair<std::string, double>> complexExprResults = {
    {{"((((1+1))))", 2.0},
     {"2^(2)*cos(0.0)", 4.0},
     {"cos(cos(3.14159/2))", 1.0}}};

const std::vector<std::string> invalidExpressions = {
    "(1+2",   "1+2)", "cos(0.0", "5-*4",    "(((1+1)+2)",
    "(5-2/)", "-",    "/1",      "1+(^2-1)"};

TEST_CASE("Expression: Result correctness") {
  SECTION("Basic Expressions") {
    for (auto pair : basicExprResults) {
      INFO("Error encountered while calculating result for expression "
           << pair.first);
      double result;
      REQUIRE_NOTHROW(result = Expression(pair.first).result());
      INFO("Calculated " << result << " for expression " << pair.first
                         << " but expected " << pair.second);
      CHECK(nearEqual(result, pair.second));
    }
  }
  SECTION("Complex Expressions") {
    for (auto pair : complexExprResults) {
      INFO("Error encountered while calculating result for expression "
           << pair.first);
      double result;
      REQUIRE_NOTHROW(result = Expression(pair.first).result());
      INFO("Calculated " << result << " for expression " << pair.first
                         << " but expected " << pair.second);
      CHECK(nearEqual(result, pair.second));
    }
  }
}

TEST_CASE("Expression: Input Validation") {
  // TODO
  for (std::string input : invalidExpressions) {
    INFO("Invalid input " << input << " erroneously marked as valid.");
    Expression expression(input);
    REQUIRE_THROWS(expression.validate());
    REQUIRE(expression.isValidated() == false);
  }
  REQUIRE(1 + 1 == 2);
}

TEST_CASE("calc: Option Parsing") {
  // Common setup for all subtests
  std::string helpStr{"TEST HELP STRING"};
  const char *programName{"calc"};
  // Redirect standard output
  std::ostringstream capturedOutput;
  auto oldCoutBuf = std::cout.rdbuf(capturedOutput.rdbuf());

  SECTION("No arguments") {
    const char *argv[] = {programName};
    ArgParser parser(helpStr);
    parser.parse(1, argv);
    REQUIRE(parser.shouldExit() == true);
    REQUIRE(parser.argString().empty());
  }

  SECTION("Passing help argument") {
    SECTION("Passing --help alone") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName, (char *)"--help"};
      ArgParser parser(helpStr);
      parser.parse(2, argv);

      REQUIRE(parser.shouldExit() == true);
      INFO("Passed --help but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }

    SECTION("Passing -h alone") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName, (char *)"-h"};
      ArgParser parser(helpStr);
      parser.parse(2, argv);

      REQUIRE(parser.shouldExit() == true);
      INFO("Passed -h but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }

    SECTION("Passing -h before other args") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName, (char *)"-h", "sqrt(4)", (char *)"+",
                            (char *)"ln(2)"};
      ArgParser parser(helpStr);
      parser.parse(5, argv);

      REQUIRE(parser.shouldExit() == true);
      INFO("Passed -h but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }

    SECTION("Passing -h before after args") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName, "sqrt(4)", (char *)"+",
                            (char *)"ln(2)", (char *)"-h"};
      ArgParser parser(helpStr);
      parser.parse(5, argv);

      REQUIRE(parser.shouldExit() == true);
      INFO("Passed -h but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }

    SECTION("Passing -h in between args") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName, "sqrt(4)", (char *)"+", (char *)"-h",
                            (char *)"ln(2)"};
      ArgParser parser(helpStr);
      parser.parse(5, argv);

      REQUIRE(parser.shouldExit() == true);
      INFO("Passed -h but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }
  }

  SECTION("Passing precision argument") {
    SECTION("Passing -p alone") {
      const char *argv[] = {programName, (char *)"-p", (char *)"3"};
      ArgParser parser(helpStr);
      parser.parse(3, argv);
      REQUIRE(parser.shouldExit() == true);
    }

    SECTION("Passing -p before expression") {
      const char *argv[] = {programName, (char *)"-p", (char *)"123",
                            (char *)"2", (char *)"+3"};
      ArgParser parser(helpStr);
      parser.parse(5, argv);
      INFO(
          "Parser received -p # <expression> but flagged shouldExit() == true");
      REQUIRE(parser.shouldExit() == false);
      INFO("Parser should have precision 123 but doesn't");
      REQUIRE(parser.precision() == 123);
      Expression expression(parser.argString());
      double result{expression.result()};
      INFO("Calculated " << result << " for expression '2 +3' but expected 5");
      REQUIRE(nearEqual(expression.result(), 5.0));
    }

    SECTION("Passing --precision after expression") {
      const char *argv[] = {programName, (char *)"2", (char *)"+3",
                            (char *)"--precision", (char *)"123"};
      ArgParser parser(helpStr);
      parser.parse(5, argv);
      INFO(
          "Parser received <expression> --precision # but flagged shouldExit() "
          "== true");
      REQUIRE(parser.shouldExit() == false);
      INFO("Parser should have precision 123 but doesn't");
      REQUIRE(parser.precision() == 123);
      Expression expression(parser.argString());
      double result{expression.result()};
      INFO("Calculated " << result << " for expression '2 +3' but expected 5");
      REQUIRE(nearEqual(expression.result(), 5.0));
    }

    SECTION("Passing --precision and --help with expression") {
      resetOstringstream(capturedOutput);
      const char *argv[] = {programName,   (char *)"--precision",
                            (char *)"123", (char *)"--help",
                            (char *)"2",   (char *)"+3"};
      ArgParser parser(helpStr);
      parser.parse(6, argv);
      INFO("Parser received --precision # --help <expression> but flagged "
           "shouldExit() == false");
      REQUIRE(parser.shouldExit() == true);
      INFO("Passed --help but help string wasn't printed");
      REQUIRE(capturedOutput.str() == helpStr + '\n');
    }
  }

  // Undo redirection of stdout buf
  std::cout.rdbuf(oldCoutBuf);
}

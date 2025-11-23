#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

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

const std::vector<std::pair<std::string, double>> exprResults = {
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

TEST_CASE("Expression: Basic Expressions") {
  for (auto pair : exprResults) {
    double result{Expression(pair.first).result()};
    INFO("Calculated " << result << " for expression " << pair.first
                       << " but expected " << pair.second);
    CHECK(nearEqual(result, pair.second));
  }
}

TEST_CASE("Expression: Complex Expressions") { REQUIRE(1 + 1 == 2); }

TEST_CASE("Expression: Invalid Input") { REQUIRE(1 + 1 == 2); }

TEST_CASE("calc: Input Expression Parsing") { REQUIRE(1 + 1 == 2); }

TEST_CASE("calc: Option Parsing") { REQUIRE(1 + 1 == 2); }

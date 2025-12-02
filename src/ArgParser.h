#pragma once

#include <iostream>
#include <string>

/******************************************************************************
 * Class for translating input arguments into a single string and enact options
 *****************************************************************************/
class ArgParser {
public:
  // Constructors
  ArgParser(std::string_view helpStr)
      : verbose(false), argStr_(), helpStr_(helpStr) {}

  // Public methods

  /// Implement option flags and concatenate remaining args into a string
  void parse(int argc, const char *const argv[]);
  /// Print ArgParser's help string
  void displayHelp() { std::cout << helpStr_ << std::endl; }
  /// Concatenated string of non-option arguments
  const std::string &argString() const { return argStr_; }
  /// Number of digits to display output numbers with
  int precision() const { return precision_; };
  /// Whether a critical problem was found during parsing
  bool shouldExit() const { return shouldExit_; };

  // Public variables
  /// Whether a 'verbose' option flag was input
  bool verbose;

private:
  // Constants

  /// Precision to use when none is supplied via an option
  static constexpr int default_precision_{6};

  // Private variables
  bool shouldExit_{false};
  std::string argStr_;
  int precision_{default_precision_};
  std::string helpStr_;
};

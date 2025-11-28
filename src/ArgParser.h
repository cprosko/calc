#pragma once

#include <iostream>
#include <string>

/******************************************************************************
 * Class for translating input arguments into a single string and enact options
 *****************************************************************************/
class ArgParser {
 public:
  ArgParser(int argc, char* argv[], std::string_view helpStr)
      : argStr_(), helpStr_(helpStr) {
    parse(argc, argv);
  }
  void parse(int argc, char* argv[]);
  void displayHelp() { std::cout << helpStr_ << std::endl; }
  std::string argString() { return argStr_; }
  int precision() { return precision_; };
  bool shouldExit() { return shouldExit_; };

 private:
  static constexpr int default_precision_{6};

  std::string parsedArg_(char* arg);

  bool shouldExit_{false};
  bool isParsed_{false};
  std::string argStr_;
  int precision_{default_precision_};
  std::string helpStr_;
};

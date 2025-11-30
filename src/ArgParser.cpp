#include "ArgParser.h"

#include <iostream>
#include <stdexcept>
#include <string>

/// Parse options in argv and concatenate remaining args into argStr_
void ArgParser::parse(int argc, const char* const argv[]) {
  if (argc == 1) {
    std::cout << "No input provided.\n";
    displayHelp();
    shouldExit_ = true;
  }
  // Concatenate remaining args into expression string
  // checking for option flags along the way
  argStr_.clear();
  for (int i{1}; i < argc; ++i) {
    std::string arg{std::string(argv[i])};
    if (arg == "-h" || arg == "--help") {
      displayHelp();
      shouldExit_ = true;
      return;
    }
    if (arg == "-p" || arg == "--precision") {
      if (i + 1 >= argc) {
        std::cerr
            << "Error: -p|--precision requires a trailing integer argument"
            << std::endl;
        shouldExit_ = true;
        return;
      }
      try {
        precision_ = std::stoi(argv[i + 1]);
      } catch (const std::invalid_argument& e) {
        std::cerr
            << "Error: -p|--precision requires a trailing integer argument"
            << std::endl;
        shouldExit_ = true;
        return;
      }
      i++;
      continue;
    }
    argStr_ += arg;
  }

  if (argStr_.empty()) {
    std::cout << "No nonoptional arguments provided.\n";
    displayHelp();
    shouldExit_ = true;
  }
  return;
}

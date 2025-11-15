# Makefile for calculator C++ project

# Specify C++ compiler: CXX is a standard variable name for specifying the
# C++ compiler
CXX := g++

# C++ Compiler flags
CXXFLAGS := -std=c++23 -Werror -Wall -Weffc++ -Wextra -Wconversion \
						-Wsign-conversion -pedantic-errors

# Target executable
TARGET := calc
DEBUG_TARGET := calc_debug

# Source files
SRC := calc.cpp Expression.cpp
DEBUG_SRC := Expression.cpp

# Object files constructed by writing all source files with a .o extension.
OBJ := $(SRC:.cpp=.o)

# Default rule
all: $(TARGET)

# Link and compile
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile .cpp files into .o files
%.o: %.cpp Expression.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug target: Build Calculator.cpp with its own main() function
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_SRC)
	$(CXX) $(CXXFLAGS) -DEXPRESSION_DEBUG -o $(DEBUG_TARGET) $(DEBUG_SRC)

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET) $(DEBUG_TARGET)

# Phony targets
.PHONY: all clean debug

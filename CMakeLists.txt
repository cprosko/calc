cmake_minimum_required(VERSION 3.28)

project(calc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Create compile_commands.json for linter
# Packages
find_package(Catch2 REQUIRED)

# Force C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add strict warning flags and treat warnings as errors
set(WARNING_FLAGS
  -Wall
  -Werror
  -Weffc++
  -Wextra
  -Wconversion
  -Wsign-conversion
  -pedantic-errors
)

# Libraries
add_library(ExpressionLogic STATIC src/Expression.cpp)
target_include_directories(ExpressionLogic PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Main Executable
add_executable(${PROJECT_NAME} src/main.cpp src/Expression.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE ExpressionLogic)
target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})

# Tests executable
add_executable(test test/test.cpp)
set_target_properties(test PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(test PRIVATE ExpressionLogic)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_compile_options(test PRIVATE ${WARNING_FLAGS})

# Debug Executable
add_executable(debug src/debug_main.cpp)
set_target_properties(debug PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(debug PRIVATE ExpressionLogic)
target_compile_options(debug PRIVATE ${WARNING_FLAGS})


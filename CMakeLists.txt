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

# Main Executable
add_executable(${PROJECT_NAME} src/main.cpp src/Expression.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})

# Tests executable
add_executable(tests src/tests.cpp)
set_target_properties(tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_compile_options(tests PRIVATE ${WARNING_FLAGS})

# Debug Executable
add_executable(debug src/Expression.cpp)
target_include_directories(debug PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(debug PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_definitions(debug PRIVATE EXPRESSION_DEBUG)
target_compile_options(debug PRIVATE ${WARNING_FLAGS})


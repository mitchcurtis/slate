# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(slate VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

option(ENABLE_TESTING "" ON)

find_package(Qt5 5.12 COMPONENTS Core Gui Quick Widgets Test REQUIRED)

add_library(
  projectWarning INTERFACE
)

target_compile_options(
  projectWarning INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic>>
  $<$<CXX_COMPILER_ID:Clang>:$<BUILD_INTERFACE:-Weverything;-Wpedantic>>
  $<$<CXX_COMPILER_ID:MSVC>:/W0>
)

add_subdirectory(lib)
add_subdirectory(app)
if(ENABLE_TESTING)
  enable_testing()

  add_subdirectory(tests)
endif()


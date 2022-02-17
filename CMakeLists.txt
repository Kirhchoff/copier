cmake_minimum_required(VERSION 3.5)
project(copier LANGUAGES CXX)

add_library(copier_lib)
target_include_directories(copier_lib PUBLIC include)
target_sources(copier_lib PRIVATE src/chunk.cpp src/metrics.cpp)

add_executable(copier main.cpp)
target_link_libraries(copier PRIVATE copier_lib)

find_package(Catch2 3 REQUIRED)
add_executable(tests tests/chunk_tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain copier_lib)
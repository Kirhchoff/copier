cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
project(copy_file LANGUAGES CXX)

add_library(copier_lib)
target_include_directories(copier_lib PUBLIC include)
target_sources(copier_lib PRIVATE src/chunk.cpp src/metrics.cpp src/app.cpp)

add_executable(copy_file main.cpp)
target_link_libraries(copy_file PRIVATE copier_lib pthread)

find_package(Catch2 3 REQUIRED)
add_executable(tests tests/chunk_tests.cpp tests/circular_buffer_tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain copier_lib)
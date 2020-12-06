cmake_minimum_required(VERSION 3.17)
project(aoc2020)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# Static linking
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# External libs
find_package(Eigen3 CONFIG REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED)

# Executables
add_executable(day1 day1.cpp day6.cpp)
add_executable(day2 day2.cpp)
add_executable(day3 day3.cpp)
target_link_libraries(day3 PRIVATE Eigen3::Eigen)
add_executable(day4 day4.cpp)
target_link_libraries(day4 PRIVATE Boost::boost)
add_executable(day5 day5.cpp)
add_executable(day6 day6.cpp)
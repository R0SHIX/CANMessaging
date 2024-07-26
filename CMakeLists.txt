cmake_minimum_required(VERSION 3.10)
project(CANTest)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_library(PCAN_LIBRARY NAMES pcanbasic)

add_executable(reader main.cpp)
target_link_libraries(reader ${PCAN_LIBRARY})

target_link_directories(reader PRIVATE /usr/include)

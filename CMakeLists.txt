cmake_minimum_required(VERSION 3.15)

project(screen2web)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(screen2web ${SRC})

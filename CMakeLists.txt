cmake_minimum_required(VERSION 3.11)
project(kisyshot)


set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(kisyshot main.cpp src/ast/token.cc src/compiler/lexer.cc)
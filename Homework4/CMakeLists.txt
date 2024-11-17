cmake_minimum_required(VERSION 3.29)
project(Homework4)

set(CMAKE_CXX_STANDARD 20)

add_executable(int int.cpp
        Interpreter.cpp
        Interpreter.h)
add_executable(asm asm.cpp
        Assembler.cpp
        Assembler.h)

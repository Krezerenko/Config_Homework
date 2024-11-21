cmake_minimum_required(VERSION 3.29)
project(Homework4)

set(CMAKE_CXX_STANDARD 20)

add_executable(int int.cpp
        Interpreter.cpp
        Interpreter.h
        Global.h)
add_executable(asm asm.cpp
        Assembler.cpp
        Assembler.h
        Global.h)

set(CMAKE_PREFIX_PATH "E:/.vcpkg-clion/vcpkg/packages")
find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(int PRIVATE tinyxml2::tinyxml2)
target_link_libraries(asm PRIVATE tinyxml2::tinyxml2)
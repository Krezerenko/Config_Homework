#include "Assembler.h"

#include <iostream>

int main(int argc, char** argv)
{
    return Assembler::Compile(std::string(argv[1]), std::string(argv[2]));
}

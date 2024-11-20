#include "Assembler.h"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Not enough arguments. Please provide a source file and an output file destination.";
        return 1;
    }
    return Assembler::Compile(std::string(argv[1]), std::string(argv[2]));
}

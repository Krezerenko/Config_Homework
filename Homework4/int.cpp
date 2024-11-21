#include "Interpreter.h"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cerr << "Not enough arguments. The interpreter takes:\n"
                     " - Path to execution(bytecode) file\n"
                     " - Path to results file\n"
                     " - Start address of output memory region\n"
                     " - End address of output memory region\n";
        return 1;
    }
    Interpreter interpreter;
    return interpreter.Execute(argv[1], argv[2],std::stoi(argv[3]),
                               std::stoi(argv[4]));
}

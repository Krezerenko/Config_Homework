#include "Assembler.h"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Not enough arguments. The assembler takes:\n"
                     " - Path to source file written in educational language\n"
                     " - Path to output binary file\n"
                     "[- Path to output log file]\n";
        return 1;
    }
    std::string logPath;
    if (argc > 3) logPath = std::string(argv[3]);
    return Assembler::Compile(std::string(argv[1]), std::string(argv[2]), logPath);
}

#include "Interpreter.h"

#include <iostream>

int main(int argc, char** argv)
{
    return Interpreter::Execute(argv[1], argv[2]);
}

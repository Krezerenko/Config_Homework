#ifndef HOMEWORK4_INTERPRETER_H
#define HOMEWORK4_INTERPRETER_H

#include "Global.h"

#include <string>
#include <bitset>
#include <vector>
#include <fstream>


class Interpreter
{
    unsigned char* m_memory;
    #define mem_i(addr) *reinterpret_cast<unsigned int*>(&m_memory[addr])

public:
    Interpreter();
    ~Interpreter();
    int Execute(const std::string &codePath, const std::string &outPath, unsigned int outputStart,
                unsigned int outputEnd);

private:
    static bool ReadCommand(std::ifstream &code, std::bitset<MaxCommandSize> &command);
    static void ReadBytes(std::ifstream &code, std::bitset<MaxCommandSize> &command, unsigned int amount);
    static void ConvertCommand(std::bitset<MaxCommandSize> &command, std::vector<unsigned int>& operands);
    static void
    BitToInt(std::bitset<MaxCommandSize> &command, const std::vector<unsigned int> &sizes,
             unsigned int byteSize, unsigned int remainder, std::vector<unsigned int> &operands);

public:
    class ReadError : std::exception
    {
    };
};


#endif //HOMEWORK4_INTERPRETER_H

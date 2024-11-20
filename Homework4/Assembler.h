#ifndef HOMEWORK4_ASSEMBLER_H
#define HOMEWORK4_ASSEMBLER_H

#include "Global.h"

#include <string>
#include <vector>
#include <bitset>

class Assembler
{
    static void ReadLine(const std::string &line, std::vector<std::string> &out);
    static std::string CompileLine(const std::vector<std::string> &tokens);
    static void IntToBit(unsigned int num, std::bitset<MaxCommandSize> &command,
                         unsigned int start, unsigned int end);
    static std::string BitToByte(const std::bitset<MaxCommandSize> &bits, unsigned int byteSize);

    static void WrongCommandArgumentsMessage(const std::string &command, unsigned int rightAmount, unsigned int wrongAmount);

public:
    static int Compile(const std::string& scriptPath, const std::string& outPath);

    class InvalidSyntax : std::exception
    {
    };
};


#endif //HOMEWORK4_ASSEMBLER_H

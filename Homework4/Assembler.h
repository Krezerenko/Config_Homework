#ifndef HOMEWORK4_ASSEMBLER_H
#define HOMEWORK4_ASSEMBLER_H

#include <string>
#include <vector>
#include <bitset>

class Assembler
{
    static constexpr unsigned int MaxCommandSizeBytes = 13;
    static constexpr unsigned int MaxCommandSize = MaxCommandSizeBytes * sizeof(char) * 8;

    static void ReadLine(const std::string &line, std::vector<std::string> &out);
    static std::string CompileLine(const std::vector<std::string> &tokens);
    static void ConvertInt(unsigned int num, std::bitset<MaxCommandSize> &buffer,
                           unsigned int start, unsigned int end);
    static std::string BitToByte(std::bitset<MaxCommandSize> bits, unsigned int byteSize);

    static void WrongCommandArgumentsMessage(const std::string &command, unsigned int rightAmount, unsigned int wrongAmount);

public:
    static int Compile(const std::string& scriptPath, const std::string& outPath);

    class InvalidSyntax : std::exception
    {
    };
};


#endif //HOMEWORK4_ASSEMBLER_H

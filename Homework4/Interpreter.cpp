#include "Interpreter.h"

#include <fstream>
#include <iostream>

int Interpreter::Execute(const std::string &codePath, const std::string &outPath)
{
    std::ifstream code(codePath, std::ios::binary | std::ios::in);
    std::bitset<MaxCommandSize> command;
    std::vector<unsigned int> operands;
    try
    {
        while(ReadCommand(code, command))
        {
            std::cout << command << '\n';
            ConvertCommand(command, operands);
            command.reset();
            for (unsigned int op : operands)
            {
                std::cout << op << ' ';
            }
            std::cout << '\n';
        }
    }
    catch (const ReadError&)
    {
        code.close();
        return 2;
    }
    code.close();
    return 0;
}

bool Interpreter::ReadCommand(std::ifstream &code, std::bitset<MaxCommandSize> &command)
{
    code.read(reinterpret_cast<char*>(&command) + MaxCommandSizeBytes - 1, 1);
    if (!code) return false;

    unsigned int commandNum = *(reinterpret_cast<unsigned char*>(&command) + MaxCommandSizeBytes - 1) >> 3;
    switch (commandNum)
    {
        case 26:
            ReadBytes(code, command, 7);
            break;
        case 4:
            ReadBytes(code, command, 9);
            break;
        case 27:
            ReadBytes(code, command, 8);
            break;
        case 10:
            ReadBytes(code, command, 13);
            break;
        default:
            std::cerr << "Read error: " + std::to_string(commandNum) + " - invalid commandNum.\n";
            throw ReadError();
    }
    return true;
}

void Interpreter::ReadBytes(std::ifstream &code, std::bitset<MaxCommandSize> &command, unsigned int amount)
{
    for (int i = 1; i < amount; ++i)
    {
        code.read(reinterpret_cast<char*>(&command) + MaxCommandSizeBytes - 1 - i, 1);
    }
}

void Interpreter::ConvertCommand(std::bitset<MaxCommandSize> &command, std::vector<unsigned int> &operands)
{
    unsigned int commandNum = *(reinterpret_cast<const unsigned char*>(&command) + MaxCommandSizeBytes - 1) >> 3;
    operands.clear();
    operands.push_back(commandNum);
    switch (commandNum)
    {
        case 26:
            BitToInt(command, {5, 19, 26}, 7, 6, operands);
            break;
        case 4:
            BitToInt(command, {5, 14, 26, 26}, 9, 1, operands);
            break;
        case 27:
            BitToInt(command, {5, 26, 26}, 8, 7, operands);
            break;
        case 10:
            BitToInt(command, {5, 26, 26, 14, 26}, 13, 7, operands);
            break;
        default:
            std::cerr << "erm what the sigma\n";
            return;
    }
}

void Interpreter::BitToInt(std::bitset<MaxCommandSize> &command, const std::vector<unsigned int> &sizes,
                           unsigned int byteSize, unsigned int remainder, std::vector<unsigned int> &operands)
{
    command >>= MaxCommandSize - byteSize * 8 + remainder;
    operands.resize(sizes.size());

    std::bitset<MaxCommandSize> mask;
    unsigned int ones = ~0;
    for (int i = sizes.size() - 1; i > 0; --i)
    {
        (mask &= 0) |= (ones % (1 << sizes[i]));
        operands[i] = (command & mask).to_ulong();
        command >>= sizes[i];
    }
}

#include "Assembler.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <bitset>

int Assembler::Compile(const std::string& scriptPath, const std::string& outPath)
{
    std::ifstream source(scriptPath);
    if (!source)
    {
        std::cerr << "Couldn't locate file on path \"" + scriptPath + "\".\n";
        return 2;
    }
    std::ofstream out(outPath, std::ios::trunc | std::ios::binary);

    std::string currentLine;
    while (std::getline(source, currentLine))
    {
        std::vector<std::string> words;
        ReadLine(currentLine, words);

        if (words.empty()) continue;
        try
        {
            out << CompileLine(words);
        }
        catch(const InvalidSyntax&)
        {
            source.close();
            out.close();
            return 3;
        }
    }

    source.close();
    out.close();
    return 0;
}

void Assembler::ReadLine(const std::string &line, std::vector<std::string> &out)
{
    std::string word;
    bool isSpace = true;
    for (char c : line)
    {
        if (c == ';')
        {
            break;
        }
        if (c == ' ' || c == '\t')
        {
            isSpace = true;
            continue;
        }
        if (isSpace)
        {
            isSpace = false;
            out.push_back(word);
            word.clear();
        }
        word.push_back(c);
    }
    out.push_back(word);
    out.erase(out.begin());
}

std::string Assembler::CompileLine(const std::vector<std::string> &tokens)
{
    std::bitset<MaxCommandSize> command;
    const std::string &name = tokens[0];
    if (name == "ldc")
    {
        if (tokens.size() != 3)
        {
            WrongCommandArgumentsMessage("ror", 2, tokens.size() - 1);
            throw InvalidSyntax();
        }
        IntToBit(26, command, 0, 5);
        IntToBit(std::stoi(tokens[1]), command, 5, 24);
        IntToBit(std::stoi(tokens[2]), command, 24, 50);
        return BitToByte(command, 7);
    }
    if (name == "rd")
    {
        if (tokens.size() != 4)
        {
            WrongCommandArgumentsMessage("rd", 3, tokens.size() - 1);
            throw InvalidSyntax();
        }
        IntToBit(4, command, 0, 5);
        IntToBit(std::stoi(tokens[1]), command, 5, 19);
        IntToBit(std::stoi(tokens[2]), command, 19, 45);
        IntToBit(std::stoi(tokens[3]), command, 45, 71);
        return BitToByte(command, 9);
    }
    if (name == "wr")
    {
        if (tokens.size() != 3)
        {
            WrongCommandArgumentsMessage("wr", 2, tokens.size() - 1);
            throw InvalidSyntax();
        }
        IntToBit(27, command, 0, 5);
        IntToBit(std::stoi(tokens[1]), command, 5, 31);
        IntToBit(std::stoi(tokens[2]), command, 31, 57);
        return BitToByte(command, 8);
    }
    if (name == "ror")
    {
        if (tokens.size() != 5)
        {
            WrongCommandArgumentsMessage("ror", 4, tokens.size() - 1);
            throw InvalidSyntax();
        }
        IntToBit(10, command, 0, 5);
        IntToBit(std::stoi(tokens[1]), command, 5, 31);
        IntToBit(std::stoi(tokens[2]), command, 31, 57);
        IntToBit(std::stoi(tokens[3]), command, 57, 71);
        IntToBit(std::stoi(tokens[4]), command, 71, 97);
        return BitToByte(command, 13);
    }

    std::cerr << "Syntax error: \"" + name + "\" - no such command.\n";
    throw InvalidSyntax();
}

void Assembler::IntToBit(unsigned int num, std::bitset<MaxCommandSize> &command, unsigned int start, unsigned int end)
{
    unsigned int size = end - start;

    std::bitset<MaxCommandSize> buffer = command;
    std::bitset<MaxCommandSize> mask;
    unsigned int ones = ~0;
    mask |= (ones % (1 << size));
    mask <<= MaxCommandSize - end;
    buffer |= num;
    buffer <<= MaxCommandSize - end;
    buffer &= mask;
    command |= buffer;
}

std::string Assembler::BitToByte(const std::bitset<MaxCommandSize> &bits, unsigned int byteSize)
{
    std::string out(byteSize, '\0');
    for (int i = 0; i < byteSize; ++i)
    {
        out[i] = *(reinterpret_cast<const char*>(&bits) + MaxCommandSizeBytes - i - 1);
    }
    return out;
}

void Assembler::WrongCommandArgumentsMessage(const std::string &command, unsigned int rightAmount, unsigned int wrongAmount)
{
    std::cout << "Syntax error: command \"" + command +
                 "\" - expected " + std::to_string(rightAmount) +
                 " arguments, provided " + std::to_string(wrongAmount) << ".\n";
}

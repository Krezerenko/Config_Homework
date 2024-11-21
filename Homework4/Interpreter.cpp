#include "Interpreter.h"

#include <fstream>
#include <iostream>
#include <tinyxml2.h>
#include <sstream>

Interpreter::Interpreter()
{
    m_memory = new unsigned char[MemorySize];
}

Interpreter::~Interpreter()
{
    delete[] m_memory;
}

int Interpreter::Execute(const std::string &codePath, const std::string &outPath, unsigned int outputStart,
                         unsigned int outputEnd)
{
    std::ifstream code(codePath, std::ios::binary | std::ios::in);
    if (!code)
    {
        std::cerr << "Couldn't locate file on path \"" + codePath + "\".\n";
        return 2;
    }
    if (outputEnd > MemorySize)
    {
        std::cerr << "End address exceeds virtual memory capacity.\n";
        return 2;
    }
    std::bitset<MaxCommandSize> command;
    std::vector<unsigned int> operands;
    try
    {
        while(ReadCommand(code, command))
        {
//            std::cout << command << '\n';
            ConvertCommand(command, operands);
            command.reset();
//            for (unsigned int op : operands)
//            {
//                std::cout << op << ' ';
//            }
//            std::cout << '\n';
            switch (operands[0])
            {
                case 26:
                    mem_i(operands[2]) = operands[1];
                    break;
                case 4:
                    m_memory[operands[3]] = m_memory[operands[2] + operands[1]];
                    break;
                case 27:
                    m_memory[operands[2]] = m_memory[operands[1]];
                    break;
                case 10:
                    mem_i(operands[2] + operands[3]) = (mem_i(operands[1]) >> mem_i(operands[4]) % (sizeof(int) * 8)) |
                            (mem_i(operands[1]) << (sizeof(int) * 8 - mem_i(operands[4]) % (sizeof(int) * 8)));
                    break;
            }
        }
    }
    catch (const ReadError&)
    {
        code.close();
        return 3;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("root");
    doc.InsertEndChild(root);
    for (unsigned int i = outputStart; i < outputEnd; ++i)
    {
        tinyxml2::XMLElement* reg = doc.NewElement(("r" + std::to_string(i)).c_str());
        std::stringstream converted;
//        converted << std::converted << static_cast<unsigned int>(m_memory[i]);
        converted << std::bitset<8>(m_memory[i]);
        reg->SetText(converted.str().c_str());
        root->InsertEndChild(reg);
    }
    doc.SaveFile(outPath.c_str());
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
            std::cerr << "Read error: " + std::to_string(commandNum) + " - invalid command number.\n";
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

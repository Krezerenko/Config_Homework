

#ifndef HOMEWORK1_COMMANDHANDLER_H
#define HOMEWORK1_COMMANDHANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class CommandHandler
{
    string _currentPath;
public:
    // Function
    void printError(const std::string& errorMessage);
    void pwd();
    void ls();
    void cat(const std::string& filename);
};


#endif //HOMEWORK1_COMMANDHANDLER_H

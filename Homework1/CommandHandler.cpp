

#include "CommandHandler.h"

void CommandHandler::printError(const string &errorMessage)
{
    cerr << errorMessage << '\n';
}

void CommandHandler::pwd()
{
    cout << "/home/user" << '\n';
}

void CommandHandler::ls()
{
    // List files
    vector<string> files = {"file1.txt", "file2.txt", "file3.txt"};

    for (const string &file: files)
    {
        cout << file << '\n';
    }
}

void CommandHandler::cat(const string &filename)
{
    // Read and display content of the specified file
    if (filename == "file1.txt")
    {
        cout << "Content of file1.txt" << '\n';
    } else if (filename == "file2.txt")
    {
        cout << "Content of file2.txt" << '\n';
    } else if (filename == "file3.txt")
    {
        cout << "Content of file3.txt" << '\n';
    } else
    {
        printError("File not found.");
    }
}
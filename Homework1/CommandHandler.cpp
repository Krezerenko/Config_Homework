

#include "CommandHandler.h"
#include <zip.h>

CommandHandler::CommandHandler(int argc, char *argv[])
{
    m_userName = argv[0];
    m_computerName = argv[1];
    m_fileSystemImagePath = argv[2];
    m_logPath = argv[3];
    if (argc > 5)
    {
        m_currentPath = argv[4] + m_userName;
    }
    else
    {
        m_currentPath = "/home/" + m_userName;
    }
}

void CommandHandler::log(const QString &message)
{

}

void CommandHandler::ls(const QString &path)
{

}

void CommandHandler::cd(const QString &path)
{
    m_currentPath = path;
    OnCurrentPathChanged(path);
}

void CommandHandler::exit()
{
    std::cout << "exit" << '\n';
}

void CommandHandler::rev(const QString &path)
{

}

void CommandHandler::history()
{

}

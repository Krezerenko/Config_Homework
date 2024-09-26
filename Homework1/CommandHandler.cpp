

#include "CommandHandler.h"
#include <zip.h>
#include <QRegularExpression>

CommandHandler::CommandHandler(int argc, char *argv[])
{
    m_userName = argv[1];
    m_computerName = argv[2];

    m_fileSystemImagePath = argv[3];
    int* errcode = new int;
    *errcode = 0;
    m_zipArchive = zip_open(m_fileSystemImagePath.toStdString().c_str(), 0, errcode);
    if (*errcode != 0)
    {
        std::cerr << "Couldn't open system image archive.\n"
                     "Error code: " << zip_strerror(m_zipArchive) << '\n';
        delete errcode;
        exit();
        return;
    }
    delete errcode;

    m_logPath = argv[4];
    m_logFile = std::fstream();

    if (argc > 5)
    {
        m_homePath = argv[5] + m_userName + '/';
    }
    else
    {
        m_homePath = "home/" + m_userName + '/';
    }
    if (zip_name_locate(m_zipArchive, m_homePath.toStdString().c_str(), 0) < 0)
    {
        zip_dir_add(m_zipArchive, m_homePath.toStdString().c_str(), 0);
    }
    m_currentPath = m_homePath;
}

CommandHandler::~CommandHandler()
{
    zip_close(m_zipArchive);
    m_logFile.close();
}


void CommandHandler::InitUi()
{
    OnCurrentPathChanged('/' + m_currentPath);
}

void CommandHandler::SetCurrentPath(const QString &path)
{
    m_currentPath = path;
    OnCurrentPathChanged('/' + m_currentPath);
}

QString CommandHandler::ToZipPath(const QString &path)
{
    if (path.isEmpty())
    {
        return m_homePath;
    }
    if (path == "/")
    {
        return "";
    }
    if (path == ".")
    {
        return m_currentPath;
    }
    if (path == "..")
    {
        QString parentPath = m_currentPath;
        parentPath.replace(QRegularExpression("[^/]*/$", QRegularExpression::MultilineOption), "");
        std::cout << parentPath.toStdString() << '\n';
        return ToZipPath('/' + parentPath);
    }

    QString finalPath;
    if (path[0] == '/')
    {
        finalPath = path;
        finalPath.replace(0, 1, "");
    }
    else
    {
        finalPath = m_currentPath + path;
    }

    if (finalPath[finalPath.size() - 1] != '/')
    {
        finalPath += '/';
    }

    return finalPath;
}

void CommandHandler::log(const QString &message)
{
    std::cout << message.toStdString() << '\n';

    m_logFile.open(m_logPath.toStdString(), std::ios::app | std::ios::out);
    m_logFile << message.toStdString() << '\n';
    m_logFile.close();
}

void CommandHandler::ls(const QString &path)
{
    for (int i = 0; i < zip_get_num_entries(m_zipArchive, 0); ++i)
    {
        QString name = zip_get_name(m_zipArchive, i, 0);
        QString wd = path == "" ? m_currentPath : ToZipPath(path);
        if (!name.startsWith(wd)) continue;

        name.replace(wd, "");
        if (name.back() == '/')
        {
            name.removeLast();
        }
        if (name.contains('/')) continue;

        std::cout << name.toStdString() << '\n';
    }
}

void CommandHandler::cd(const QString &path)
{
    QString zipPath = ToZipPath(path);
    if (zipPath != "" && zip_name_locate(m_zipArchive, zipPath.toStdString().c_str(), 0) < 0)
    {
        log("Can not change directory. No directory found.");
        return;
    }
    log("Changed location to folder /" + zipPath);
    SetCurrentPath(zipPath);
}

void CommandHandler::exit()
{
//    zip_close(m_zipArchive);
    std::cout << "exit" << '\n';
    ::exit(0);
}

void CommandHandler::rev(const QString &path)
{

}

void CommandHandler::history()
{

}



#include "CommandHandler.h"
#include "ZipHandler.h"
#include <QRegularExpression>

CommandHandler::CommandHandler(int argc, char *argv[]) : m_zip(QString(argv[3]))
{
    m_userName = argv[1];
    m_computerName = argv[2];

    m_logPath = argv[4];
    m_logFile = std::fstream();

    m_cmdLogPath = "var/log/cmdlog.txt";

    if (argc > 5)
    {
        m_homePath = argv[5] + m_userName + '/';
    }
    else
    {
        m_homePath = "home/" + m_userName + '/';
    }
    if (!m_zip.FindFile(m_homePath))
    {
        m_zip.AddFile(m_homePath);
    }
    m_currentPath = m_homePath;
}

CommandHandler::~CommandHandler()
{
    CleanUp();
}

void CommandHandler::CleanUp()
{
//    if (m_zipWrite != nullptr)
//        zipClose(m_zipWrite, nullptr);
//    if (m_zipRead != nullptr)
//        unzClose(m_zipRead);
//    m_zipWrite = nullptr;
//    m_zipRead = nullptr;
//    m_logFile.close();
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

QString CommandHandler::ToZipPath(const QString &path) const
{
    if (path.isEmpty())
    {
        return m_homePath;
    }
    if (path[0] == '~')
    {
        QString finalPath = path;
        finalPath.removeAt(0);
        finalPath = m_homePath + finalPath;
        return ToZipPath(finalPath);
    }
    if (path == "/")
    {
        return "";
    }

    QString finalPath;
    if (path[0] == '/')
    {
        finalPath = path;
        finalPath.removeAt(0);
    }
    else
    {
        finalPath = m_currentPath + path;
    }

    if (finalPath[finalPath.size() - 1] != '/')
    {
        finalPath += '/';
    }
    if (finalPath.contains(".."))
    {
        auto regexp = QRegularExpression("([^/]*/)?\\.\\./", QRegularExpression::MultilineOption);
        auto match = regexp.match(finalPath);
        if (match.hasCaptured(0))
        {
            finalPath.replace(match.capturedStart(), match.capturedLength(), "");
            return ToZipPath('/' + finalPath);
        }
    }
    if (path.contains('.'))
    {
        finalPath.replace(QRegularExpression("(?<=(^|/))\\./", QRegularExpression::MultilineOption), "");
    }

    return finalPath;
}

void CommandHandler::Log(const QString &message)
{
    std::cout << message.toStdString() << '\n';

    m_logFile.open(m_logPath.toStdString(), std::ios::app | std::ios::out);
    m_logFile << message.toStdString() << '\n';
    m_logFile.close();
}

void CommandHandler::CmdLog(const QString &cmd)
{
    m_zip.WriteToFile(m_cmdLogPath, cmd + '\n');
}

void CommandHandler::ls(const QString &path)
{
    CmdLog("ls " + path);

    std::vector<QString> names;
    names.reserve(10);
    m_zip.GetFilesInDirectory(ToZipPath(path), names);
    for (const QString& name : names)
    {
        std::cout << name.toStdString() << '\n';
    }
}

void CommandHandler::cd(const QString &path)
{
    CmdLog("cd " + path);
    QString zipPath = ToZipPath(path);
    if (zipPath != "" && !m_zip.FindFile(zipPath))
    {
        Log("Can not change working directory. \"" + zipPath + "\" - directory does not exist.");
        return;
    }
    Log("Changed location to folder /" + zipPath);
    SetCurrentPath(zipPath);
}

void CommandHandler::exit()
{
    CmdLog("exit");
    ::exit(0);
}

void CommandHandler::rev(const QString &path)
{
    CmdLog("rev " + path);
}

void CommandHandler::history()
{
    CmdLog("history");
}

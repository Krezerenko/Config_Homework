

#include "CommandHandler.h"
//#include <zip.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#include <QRegularExpression>

CommandHandler::CommandHandler(int argc, char *argv[])
{
    m_userName = argv[1];
    m_computerName = argv[2];

    m_zipPath = argv[3];
    m_zipRead = unzOpen(m_zipPath.toStdString().c_str());
    if (m_zipRead == nullptr)
    {
        std::cerr << "Couldn't open system image archive.\n";
        exit();
        return;
    }
    unzClose(m_zipRead);

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
    zip_fileinfo zipInfo;
    if (!CheckPathExists(m_homePath))
    {
        m_zipWrite = zipOpen(m_zipPath.toStdString().c_str(), APPEND_STATUS_ADDINZIP);
        zipOpenNewFileInZip(m_zipWrite, m_homePath.toStdString().c_str(), &zipInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
        zipClose(m_zipWrite, nullptr);
    }
    m_currentPath = m_homePath;
    m_zipWrite = nullptr;
    m_zipRead = nullptr;
}

bool CommandHandler::CheckPathExists(const QString &path)
{
    m_zipRead = unzOpen(m_zipPath.toStdString().c_str());

    if (unzGoToFirstFile(m_zipRead) != UNZ_OK)
    {
        std::cerr << unzGoToFirstFile(m_zipRead) << '\n';
        unzClose(m_zipRead);
        return false;
    }
    do
    {
        char filenameInZip[256];
        unz_file_info fileInfo;

        if (unzGetCurrentFileInfo(m_zipRead, &fileInfo, filenameInZip, sizeof(filenameInZip), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            continue;
        }

        if (QString(filenameInZip) == path)
        {
            unzClose(m_zipRead);
            return true;
        }
    }
    while (unzGoToNextFile(m_zipRead) == UNZ_OK);

    unzClose(m_zipRead);
    return false;
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
    m_zipWrite = zipOpen(m_zipPath.toStdString().c_str(), APPEND_STATUS_ADDINZIP);
    zipOpenNewFileInZip(m_zipWrite, m_cmdLogPath.toStdString().c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    zipWriteInFileInZip(m_zipWrite, (cmd + '\n').toStdString().c_str(), cmd.size() + 1);
    zipCloseFileInZip(m_zipWrite);
    zipClose(m_zipWrite, nullptr);
}

void CommandHandler::ls(const QString &path)
{
    CmdLog("ls " + path);

    m_zipRead = unzOpen(m_zipPath.toStdString().c_str());
    QString wd = path == "" ? m_currentPath : ToZipPath(path);
    if (unzGoToFirstFile(m_zipRead) != UNZ_OK)
    {
        unzClose(m_zipRead);
        return;
    }
    do
    {
        char cName[256];
        unz_file_info fileInfo;

        if (unzGetCurrentFileInfo(m_zipRead, &fileInfo, cName, sizeof(cName), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            continue;
        }

        QString name = QString::fromStdString(std::string(cName, fileInfo.size_filename));
//        std::cout << name.toStdString() << '\n';
        if (!name.startsWith(wd)) continue;
        if (name == wd) continue;

//        std::cout << wd.toStdString() << ' ' << name.toStdString() << '\n';
        name.replace(wd, "");
        if (name.back() == '/')
        {
            name.removeLast();
        }
        if (name.contains('/')) continue;

        std::cout << name.toStdString() << '\n';
    }
    while (unzGoToNextFile(m_zipRead) == UNZ_OK);

    unzClose(m_zipRead);
}

void CommandHandler::cd(const QString &path)
{
    CmdLog("cd " + path);
    QString zipPath = ToZipPath(path);
    if (zipPath != "" && !CheckPathExists(zipPath))
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

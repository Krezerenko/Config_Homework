

#include "CommandHandler.h"
//#include <zip.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#include <QRegularExpression>

CommandHandler::CommandHandler(int argc, char *argv[])
{
    m_userName = argv[1];
    m_computerName = argv[2];

    m_fileSystemImagePath = argv[3];
    m_zipArchive = zipOpen(m_fileSystemImagePath.toStdString().c_str(), APPEND_STATUS_ADDINZIP);
    m_unzZipArchive = unzOpen(m_fileSystemImagePath.toStdString().c_str());
    if (m_zipArchive == nullptr)
    {
        std::cerr << "Couldn't open system image archive.\n";
        exit();
        return;
    }

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
        zipOpenNewFileInZip(m_zipArchive, m_homePath.toStdString().c_str(), &zipInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    }
    m_currentPath = m_homePath;
}

bool CommandHandler::CheckPathExists(const QString &path) const
{
    if (unzGoToFirstFile(m_unzZipArchive) != UNZ_OK)
    {
        return false;
    }
    do
    {
        char filenameInZip[256];
        unz_file_info fileInfo;

        if (unzGetCurrentFileInfo(m_unzZipArchive, &fileInfo, filenameInZip, sizeof(filenameInZip), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            continue;
        }

        if (QString(filenameInZip) == path)
        {
            return true;
        }
    }
    while (unzGoToNextFile(m_unzZipArchive) == UNZ_OK);
    return false;
}

CommandHandler::~CommandHandler()
{
    zipClose(m_zipArchive, nullptr);
    unzClose(m_unzZipArchive);
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
//    ZIP_ER_INVAL
//    EINPROGRESS
//    zip_file *cmdLogFileR = zip_fopen(m_zipArchive, m_cmdLogPath.toStdString().c_str(), 0);
//    char* buffer = new char[1024];
//    unsigned int readBytes = zip_fread(cmdLogFileR, buffer, 1024);
//    zip_source_t *cmdLogFile = zip_source_buffer(m_zipArchive, buffer, readBytes, 0);
//    delete[] buffer;
//    zip_fclose(cmdLogFileR);
//    std::cerr << zip_get_error(m_zipArchive)->zip_err << '\n';
//    zip_source_begin_write_cloning(cmdLogFile, readBytes);
//    zip_source_write(cmdLogFile, (cmd + '\n').toStdString().c_str(), cmd.size() + 1);
//    zip_source_commit_write(cmdLogFile);
//    zip_file_add(m_zipArchive, m_cmdLogPath.toStdString().c_str(), cmdLogFile, ZIP_FL_OVERWRITE);
//    zip_source_close(cmdLogFile);
//    zip_source_free(cmdLogFile);
}

void CommandHandler::ls(const QString &path)
{
    CmdLog("ls " + path);
    QString wd = path == "" ? m_currentPath : ToZipPath(path);
    if (unzGoToFirstFile(m_unzZipArchive) != UNZ_OK)
    {
        return;
    }
    do
    {
        char cName[256];
        unz_file_info fileInfo;

        if (unzGetCurrentFileInfo(m_unzZipArchive, &fileInfo, cName, sizeof(cName), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            return;
        }

        QString name = cName;
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
    while (unzGoToNextFile(m_unzZipArchive) == UNZ_OK);
}

void CommandHandler::cd(const QString &path)
{
    CmdLog("cd " + path);
    QString zipPath = ToZipPath(path);
    if (zipPath != "" && !CheckPathExists(zipPath))
    {
        Log("Can not change directory. No directory found.");
        return;
    }
    Log("Changed location to folder /" + zipPath);
    SetCurrentPath(zipPath);
}

void CommandHandler::exit()
{
    CmdLog("exit");
    std::cout << "exit" << '\n';
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

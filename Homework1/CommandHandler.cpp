

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
    m_zip.AddFile(m_cmdLogPath);

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


void CommandHandler::InitUi()
{
    OnCurrentPathChanged('/' + m_currentPath);
}

void CommandHandler::SetCurrentPath(const QString &path)
{
    m_currentPath = path;
    emit OnCurrentPathChanged('/' + m_currentPath);
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

    QString zipPath = path == "" ? m_currentPath : ToZipPath(path);
    std::vector<QString> names;
    names.reserve(10);
    m_zip.GetFilesInDirectory(zipPath, names);
    QString output;
    for (const QString& name : names)
    {
        output += name + '\n';
    }
    emit OnOutput(output);
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

void CommandHandler::cat(const QString &input)
{
    CmdLog("cat " + input);
    QVector<QString> paths;
    QString currentPath = "";
    unsigned int quoteCount = 0;
    for (int i = 0; i < input.size(); ++i)
    {
        if (input[i] == '\"')
        {
            if (!(++quoteCount % 2))
            {
                paths += currentPath;
                currentPath.clear();
            }
            continue;
        }
        if (input[i] == ' ' && !(quoteCount % 2))
        {
            if (currentPath.isEmpty()) continue;
            paths += currentPath;
            currentPath.clear();
            continue;
        }
        currentPath += input[i];
    }
    paths += currentPath;

    QString output;
    for (const QString &path : paths)
    {
        QString zipPath = ToZipPath(path);
        if (input == "" || input.back() == '/' && m_zip.FindFile(zipPath))
        {
            Log("Cat failed: \"" + zipPath + "\" is not a file.");
            continue;
        }
        if (!m_zip.FindFile(zipPath.removeLast()))
        {
            Log("Cat failed: \"" + zipPath + "\" - file not found.");
            continue;
        }
        QString data = m_zip.ReadFile(zipPath);
        if (data.size() == 0)
        {
            continue;
        }
        output += data;
    }

    emit OnOutput(output);
}

void CommandHandler::exit()
{
    CmdLog("exit");
    ::exit(0);
}

void CommandHandler::history()
{
    CmdLog("history");
    QString history = m_zip.ReadFile(m_cmdLogPath);
    OnOutput(history);
}

void CommandHandler::rev(const QString &filePath)
{
    CmdLog("rev " + filePath);
    QString zipPath = ToZipPath(filePath);
    if (filePath == "" || filePath.back() == '/' && m_zip.FindFile(zipPath))
    {
        Log("Rev failed: \"" + zipPath + "\" is not a file.");
        return;
    }
    if (!m_zip.FindFile(zipPath.removeLast()))
    {
        Log("Rev failed: \"" + zipPath + "\" - file not found.");
        return;
    }
    QString data = m_zip.ReadFile(zipPath);
    if (data.size() == 0)
    {
        emit OnOutput("");
        return;
    }
    QString reversed;
    reversed.resize(data.size());
    for (int i = 0; i < data.size(); ++i)
    {
        reversed[i] = data[data.size() - 1 - i];
    }
    emit OnOutput(reversed);
}


void CommandHandler::HandleLsButtonPressed(const QString &inputPath)
{
    ls(inputPath);
}

void CommandHandler::HandleCdButtonPressed(const QString &inputPath)
{
    cd(inputPath);
}

void CommandHandler::HandleHistoryButtonPressed()
{
    history();
}

void CommandHandler::HandleCatButtonPressed(const QString &inputPath)
{
    cat(inputPath);
}

void CommandHandler::HandleRevButtonPressed(const QString &inputPath)
{
    rev(inputPath);
}



#ifndef HOMEWORK1_COMMANDHANDLER_H
#define HOMEWORK1_COMMANDHANDLER_H

#include <iostream>
#include <fstream>
#include <QObject>
#include "ZipHandler.h"
#include <QString>

class CommandHandler : public QObject
{

Q_OBJECT

    QString m_userName;
    QString m_computerName;
    QString m_logPath;

    ZipHandler m_zip;
    std::fstream m_logFile;

    QString m_currentPath;
    QString m_cmdLogPath;
    QString m_homePath;

    void Log(const QString &message);
    void CmdLog(const QString &cmd);
//    bool CheckPathExists(const QString &path);
    QString ToZipPath(const QString &path) const;
public:
    CommandHandler(int argc, char *argv[]);
    ~CommandHandler() override;
    void CleanUp();
    void InitUi();
    void SetCurrentPath(const QString& path);

    void ls(const QString &path);
    void cd(const QString &path);
    void exit();
    void rev(const QString &path);
    void history();
signals:
    void OnCurrentPathChanged(const QString &path);

};


#endif //HOMEWORK1_COMMANDHANDLER_H

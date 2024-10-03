

#ifndef HOMEWORK1_COMMANDHANDLER_H
#define HOMEWORK1_COMMANDHANDLER_H

#include <iostream>
#include <fstream>
#include <QObject>
#include "ZipHandler.h"
#include "mainwindow.h"
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
    void InitUi();
    void SetCurrentPath(const QString& path);

    void ls(const QString &path);
    void cd(const QString &path);
    void cat(const QString &input);
    void rev(const QString &filePath);
    void history();

public slots:
    void exit();
    void HandleLsButtonPressed(const QString &inputPath);
    void HandleCdButtonPressed(const QString &inputPath);
    void HandleHistoryButtonPressed();
    void HandleCatButtonPressed(const QString &inputPath);
    void HandleRevButtonPressed(const QString &inputPath);

signals:
    void OnCurrentPathChanged(const QString &newPath);
    void OnOutput(const QString &output);
};


#endif //HOMEWORK1_COMMANDHANDLER_H



#ifndef HOMEWORK1_COMMANDHANDLER_H
#define HOMEWORK1_COMMANDHANDLER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <QObject>

class CommandHandler : public QObject
{

Q_OBJECT

    QString m_userName;
    QString m_computerName;
    QString m_fileSystemImagePath;
    QString m_logPath;

    QString m_currentPath;

    void log(const QString &message);
public:
    CommandHandler(int argc, char *argv[]);
    void ls(const QString &path);
    void cd(const QString &path);
    void exit();
    void rev(const QString &path);
    void history();
signals:
    void OnCurrentPathChanged(const QString &path);
};


#endif //HOMEWORK1_COMMANDHANDLER_H

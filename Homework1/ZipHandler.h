

#ifndef HOMEWORK1_ZIPHANDLER_H
#define HOMEWORK1_ZIPHANDLER_H

#include <QString>
#include <fstream>
#include <minizip/zip.h>
#include <minizip/unzip.h>


class ZipHandler
{

    static constexpr unsigned int NameLenSize = sizeof(unsigned int);
    static constexpr unsigned int FileLenSize = sizeof(unsigned long);

    bool m_noErrors;
    QString m_zipPath;
    QString m_zipCopyPath;
    QString m_unzipPath;
    static QString m_GenerateTempFileName();
    bool m_ApplyChanges();
public:
    QString m_Unzip();
    explicit ZipHandler(const QString &zipPath);
    ~ZipHandler();
    bool FindFile(const QString &path);
    bool WriteToFile();
};


#endif //HOMEWORK1_ZIPHANDLER_H

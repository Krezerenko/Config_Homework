

#ifndef HOMEWORK1_ZIPHANDLER_H
#define HOMEWORK1_ZIPHANDLER_H


#include <fstream>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <QString>


class ZipHandler
{
private:
    static constexpr unsigned int NameLenSize = sizeof(unsigned int);
    static constexpr unsigned int FileLenSize = sizeof(unsigned long);

    class Zipper;
    class Unzipper;

    Unzipper *m_pUnzipper;
    Zipper *m_pZipper;

    bool m_noErrors;
    QString m_zipPath;
    QString m_zipCopyPath;
    QString m_unzipPath;

    static QString m_GenerateTempFileName();
public:
    explicit ZipHandler(const QString &zipPath);
    ~ZipHandler();
    bool FindFile(const QString &path);
    bool WriteToFile();
};


#endif //HOMEWORK1_ZIPHANDLER_H

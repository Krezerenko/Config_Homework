

#ifndef HOMEWORK1_ZIPHANDLER_H
#define HOMEWORK1_ZIPHANDLER_H


//#include <fstream>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <QString>
#include <vector>


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
    void m_ReplaceOriginal();
    void m_RemoveFiles();
public:
    explicit ZipHandler(const QString &zipPath);
//    ZipHandler();
    ~ZipHandler();
    bool OpenArchive(const QString &zipPath);
    bool FindFile(const QString &path);
    bool GetFilesInDirectory(const QString &directoryPath, std::vector<QString> &buffer);
    bool WriteToFile(const QString &fileName, const QString &data);
    bool AddFile(const QString &fileName);
};


#endif //HOMEWORK1_ZIPHANDLER_H

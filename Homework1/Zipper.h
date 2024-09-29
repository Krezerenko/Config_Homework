

#ifndef HOMEWORK1_ZIPPER_H
#define HOMEWORK1_ZIPPER_H

#include <QFile>
#include "ZipHandler.h"
#include <vector>


class ZipHandler::Zipper
{
    bool m_noErrors;
    QString m_zipPath;
    QString m_unzipPath;
    QString m_zipCopyPath;

    zipFile m_currentZip;
    bool m_zipIsOpen;
    bool m_zipFileIsOpen;
    bool m_CreateAndOpen();
    bool m_OpenExisting(const QString &zipPath);
    bool m_Close();
    bool m_OpenNewFile(const QString &pathName);
    bool m_CloseCurrentFileNoRec();
    bool m_CloseCurrentFile();
    bool m_WriteInCurrentFile(char* fileBuffer, unsigned int fileBufferSize);
    bool m_AppendFileToCurrentZip(QFile &source, const QString &filename, char* fileBuffer, unsigned int fileBufferSize);
    bool m_FillCurrentZip(QFile &source);
    bool m_FillCurrentZipExcept(QFile &source, const std::vector<const QString> &exclude);
//    bool m_FillCurrentZipOnly(QFile &source, const std::vector<const QString> &include);

    void m_RemoveFiles();
public:
    explicit Zipper();
    ~Zipper();

    QString ZipNoChange(const QString &unzipPath);
    QString ZipCreateFile(const QString &unzipPath);
};


#endif //HOMEWORK1_ZIPPER_H

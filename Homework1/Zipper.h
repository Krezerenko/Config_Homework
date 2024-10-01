

#ifndef HOMEWORK1_ZIPPER_H
#define HOMEWORK1_ZIPPER_H

#include <QFile>
#include "ZipHandler.h"


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
    bool m_WriteInCurrentFile(const char *fileBuffer, unsigned int fileBufferSize);
    bool m_WriteInCurrentFileFromUnzip(QFile &source, char *fileBuffer, unsigned int fileBufferSize);
    bool m_AppendFileToCurrentZipFromUnzip(QFile &source, const QString &filename, char* fileBuffer, unsigned int fileBufferSize);
    bool m_FillCurrentZip(QFile &source);
    bool m_FillCurrentZipExceptFile(QFile &source, const QString &fileName);
    bool m_FillCurrentZipAndAppendToFile(QFile &source, const QString &fileName, const char *data, unsigned int dataSize);
//    bool m_FillCurrentZipOnly(QFile &source, const std::vector<const QString> &include);

public:
    explicit Zipper();
    ~Zipper();

    QString ZipNoChange(const QString &unzipPath);
    QString ZipAddNewFileRaw(const QString &unzipPath, const QString &fileName, const char *fileData, unsigned int fileSize);
    QString ZipAddFileNoCheck(const QString &fileName, const char *fileData, unsigned int fileSize);
    QString ZipWriteToFileRaw(const QString &unzipPath, const QString &fileName, const char* fileData, unsigned int fileSize);
    QString ZipRemoveFile(const QString &unzipPath, const QString &fileName);
    void RemoveFiles();
};


#endif //HOMEWORK1_ZIPPER_H

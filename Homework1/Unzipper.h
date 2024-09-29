

#ifndef HOMEWORK1_UNZIPPER_H
#define HOMEWORK1_UNZIPPER_H

#include "ZipHandler.h"

class ZipHandler::Unzipper
{
    bool m_noErrors;
    QString m_zipPath;
    QString m_unzipPath;

    zipFile m_currentZip;
    bool m_zipIsOpen;
    bool m_zipFileIsOpen;
    bool m_Open(const QString &zipPath);
    bool m_Close();
    bool m_GoToFirstFile();
    bool m_GoToNextFile();
    bool m_GetCurrentFileInfo(unz_file_info* info, char* nameBuffer, unsigned int nameBufferSize);
    bool m_OpenCurrentFile();
    bool m_CloseCurrentFileNoRec();
    bool m_CloseCurrentFile();
    int m_ReadCurrentFile(char* fileBuffer, unsigned int fileBufferSize);

    void m_RemoveFiles();
public:
    explicit Unzipper(const QString &zipPath);
    ~Unzipper();

    QString Unzip();
    bool FindFile(const QString &path);
};


#endif //HOMEWORK1_UNZIPPER_H

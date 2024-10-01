

#include "Zipper.h"

#include <iostream>
#include <QFile>
#include <QRegularExpression>

ZipHandler::Zipper::Zipper()
{
    m_currentZip = nullptr;
    m_zipIsOpen = false;
    m_zipFileIsOpen = false;
    m_noErrors = true;
}

ZipHandler::Zipper::~Zipper()
{
    m_Close();
    RemoveFiles();
}

bool ZipHandler::Zipper::m_CreateAndOpen()
{
    if (m_zipIsOpen) return true;

    m_noErrors = true;
    QString zipName = m_GenerateTempFileName() + ".zip";
    m_currentZip = zipOpen(zipName.toStdString().c_str(), APPEND_STATUS_CREATE);
    if (m_currentZip == nullptr)
    {
        std::cerr << "Failed to create archive for editing.\n";
        m_noErrors = false;
        m_zipIsOpen = false;
        m_zipCopyPath = "";
        return false;
    }
    m_zipIsOpen = true;
    m_zipCopyPath = zipName;
    return true;
}

bool ZipHandler::Zipper::m_OpenExisting(const QString &zipPath)
{
    if (m_zipIsOpen) return true;

    m_noErrors = true;
    m_currentZip = zipOpen(zipPath.toStdString().c_str(), APPEND_STATUS_ADDINZIP);
    if (m_currentZip == nullptr)
    {
        std::cerr << "Failed to open archive for editing.\n";
        m_noErrors = false;
        m_zipIsOpen = false;
        m_zipCopyPath = "";
        return false;
    }
    m_zipIsOpen = true;
    m_zipCopyPath = zipPath;
    return true;
}

bool ZipHandler::Zipper::m_Close()
{
    int errorCode;
    if (!m_zipIsOpen) return true;
    m_CloseCurrentFileNoRec();
    errorCode = zipClose(m_currentZip, nullptr);
    if (errorCode != ZIP_OK)
    {
        std::cerr << errorCode << ": Archive closed incorrectly.\n";
        m_currentZip = nullptr;
        m_noErrors = false;
        m_zipIsOpen = false;

        return false;
    }
    m_currentZip = nullptr;
    m_zipIsOpen = false;
    m_zipFileIsOpen = false;
    return true;
}

bool ZipHandler::Zipper::m_OpenNewFile(const QString &pathName)
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to create new file in archive: Archive is not open for writing.\n";
        return false;
    }
    if (m_zipFileIsOpen)
    {
        std::cerr << "Failed to create new file in archive: Close current file first.\n";
        return false;
    }
    zip_fileinfo info;
    info.dosDate = 0;
    int errorCode = zipOpenNewFileInZip(m_currentZip, pathName.toStdString().c_str(),
                                        &info, nullptr, 0,
                                        nullptr, 0,nullptr,
                                        Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    if (errorCode != ZIP_OK)
    {
        std::cerr << errorCode << ": Failed to create new file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = true;
    return true;
}

bool ZipHandler::Zipper::m_CloseCurrentFileNoRec()
{
    if (!m_zipFileIsOpen) return true;
    int errorCode = zipCloseFileInZip(m_currentZip);
    if (errorCode != ZIP_OK)
    {
        std::cerr << errorCode << ": Failed to close file in archive.\n";
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = false;
    return true;
}

bool ZipHandler::Zipper::m_CloseCurrentFile()
{
    if (!m_zipFileIsOpen) return true;
    if (!m_zipIsOpen) return true;
    int errorCode = zipCloseFileInZip(m_currentZip);
    if (errorCode != ZIP_OK)
    {
        std::cerr << errorCode << ": Failed to close file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = false;
    return true;
}

bool ZipHandler::Zipper::m_WriteInCurrentFile(const char *fileBuffer, unsigned int fileBufferSize)
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to write to file in archive: Archive is not open for writing.\n";
        return false;
    }
    if (!m_zipFileIsOpen)
    {
        std::cerr << "Failed to write to file in archive: File is not open.\n";
        return false;
    }

    int errorCode = zipWriteInFileInZip(m_currentZip, fileBuffer, fileBufferSize);
    if (errorCode != ZIP_OK)
    {
        std::cerr << errorCode << ": Failed to write to file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    return true;
}

bool ZipHandler::Zipper::m_WriteInCurrentFileFromUnzip(QFile &source, char *fileBuffer, unsigned int fileBufferSize)
{
    if (!m_noErrors) return false;

    unsigned int filePortionSize;
    unsigned long fileSize;

    source.read(reinterpret_cast<char*>(&fileSize), FileLenSize);
    filePortionSize = fileSize;
    while (filePortionSize > fileBufferSize)
    {
        source.read(fileBuffer, fileBufferSize);
        m_WriteInCurrentFile(fileBuffer, fileBufferSize);
        filePortionSize -= fileBufferSize;
    }
    source.read(fileBuffer, filePortionSize);
    m_WriteInCurrentFile(fileBuffer, filePortionSize);

    return true;
}

bool ZipHandler::Zipper::m_AppendFileToCurrentZipFromUnzip(QFile &source, const QString &filename, char *fileBuffer,
                                                           unsigned int fileBufferSize)
{
    if (!m_OpenNewFile(filename)) return false;

    m_WriteInCurrentFileFromUnzip(source, fileBuffer, fileBufferSize);

    return m_CloseCurrentFile();
}

bool ZipHandler::Zipper::m_FillCurrentZip(QFile &source)
{
    constexpr unsigned int nameBufferSize = 1024;
    constexpr unsigned int fileBufferSize = 1024 * 8;
    char nameBuffer[nameBufferSize];
    char fileBuffer[fileBufferSize];
    unsigned int nameSize;

    QString name;
    do
    {
        source.read(reinterpret_cast<char*>(&nameSize), NameLenSize);
        if (nameSize == 0)
        {
            return true;
        }
        source.read(nameBuffer, nameSize);
        name = QString::fromUtf8(nameBuffer, nameSize);

        if (!m_AppendFileToCurrentZipFromUnzip(source, name, fileBuffer, fileBufferSize))
        {
            return false;
        }
    }
    while (!source.atEnd());
    return true;
}

bool ZipHandler::Zipper::m_FillCurrentZipExceptFile(QFile &source, const QString &fileName)
{
    constexpr unsigned int nameBufferSize = 1024;
    constexpr unsigned int fileBufferSize = 1024 * 8;
    char nameBuffer[nameBufferSize];
    char fileBuffer[fileBufferSize];
    unsigned int nameSize;

    QString name;
    do
    {
        source.read(reinterpret_cast<char*>(&nameSize), NameLenSize);
        if (nameSize == 0)
        {
            return true;
        }
        source.read(nameBuffer, nameSize);
        name = QString::fromUtf8(nameBuffer, nameSize);

        if (fileName == name)
        {
            unsigned long fileSize;
            source.read(reinterpret_cast<char*>(&fileSize), FileLenSize);
            source.seek(source.pos() + fileSize);
            continue;
        }
        if (!m_AppendFileToCurrentZipFromUnzip(source, name, fileBuffer, fileBufferSize))
        {
            return false;
        }
    }
    while (!source.atEnd());
    return true;
}

bool ZipHandler::Zipper::m_FillCurrentZipAndAppendToFile(QFile &source, const QString &fileName, const char *data,
                                                         unsigned int dataSize)
{
    constexpr unsigned int nameBufferSize = 1024;
    constexpr unsigned int fileBufferSize = 1024 * 8;
    char nameBuffer[nameBufferSize];
    char fileBuffer[fileBufferSize];
    unsigned int nameSize;

    QString name;
    do
    {
        source.read(reinterpret_cast<char*>(&nameSize), NameLenSize);
        if (nameSize == 0)
        {
            return true;
        }
        source.read(nameBuffer, nameSize);
        name = QString::fromUtf8(nameBuffer, nameSize);

        if (fileName == name)
        {
            m_OpenNewFile(name);

            if (!m_WriteInCurrentFileFromUnzip(source, fileBuffer, fileBufferSize))
            {
                return false;
            }
            m_WriteInCurrentFile(data, dataSize);

            m_CloseCurrentFile();
            continue;
        }

        if (!m_AppendFileToCurrentZipFromUnzip(source, name, fileBuffer, fileBufferSize))
        {
            return false;
        }
    }
    while (!source.atEnd());
    return true;
}

void ZipHandler::Zipper::RemoveFiles()
{
    if (!m_unzipPath.isEmpty())
    {
        QFile file(m_unzipPath);
        file.remove();
        m_unzipPath = "";
    }
    if (!m_zipCopyPath.isEmpty())
    {
        m_Close();
        QFile file(m_zipCopyPath);
        file.remove();
        m_zipCopyPath = "";
    }
}

QString ZipHandler::Zipper::ZipNoChange(const QString &unzipPath)
{
    m_CreateAndOpen();

    QFile unzipFile(unzipPath);
    if (!unzipFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Failed to open unzipped archive file.\n";
        m_Close();
        unzipFile.close();
        m_noErrors = false;
        return "";
    }

    m_FillCurrentZip(unzipFile);

    m_Close();
    return m_zipCopyPath;
}

QString ZipHandler::Zipper::ZipAddNewFileRaw(const QString &unzipPath, const QString &fileName, const char *fileData,
                                             unsigned int fileSize)
{
    m_CreateAndOpen();

    QFile unzipFile(unzipPath);
    if (!unzipFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Failed to open unzipped archive file.\n";
        m_Close();
        unzipFile.close();
        m_noErrors = false;
        return "";
    }

    m_FillCurrentZipExceptFile(unzipFile, fileName);

    m_OpenNewFile(fileName);
    if (fileData != nullptr && fileSize > 0)
    {
        m_WriteInCurrentFile(fileData, fileSize);
    }
    m_CloseCurrentFile();

    m_Close();
    return m_zipCopyPath;
}

QString ZipHandler::Zipper::ZipAddFileNoCheck(const QString &fileName, const char *fileData, unsigned int fileSize)
{
    m_OpenExisting(m_zipCopyPath);

    m_OpenNewFile(fileName);
    if (fileData != nullptr && fileSize > 0)
    {
        m_WriteInCurrentFile(fileData, fileSize);
    }
    m_CloseCurrentFile();

    m_Close();

    return m_zipCopyPath;
}

QString ZipHandler::Zipper::ZipWriteToFileRaw(const QString &unzipPath, const QString &fileName, const char *fileData,
                                              unsigned int fileSize)
{
    m_CreateAndOpen();

    QFile unzipFile(unzipPath);
    if (!unzipFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Failed to open unzipped archive file.\n";
        m_Close();
        unzipFile.close();
        m_noErrors = false;
        return "";
    }

    m_FillCurrentZipAndAppendToFile(unzipFile, fileName, fileData, fileSize);

    m_Close();
    return m_zipCopyPath;
}

QString ZipHandler::Zipper::ZipRemoveFile(const QString &unzipPath, const QString &fileName)
{
    m_CreateAndOpen();

    QFile unzipFile(unzipPath);
    if (!unzipFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Failed to open unzipped archive file.\n";
        m_Close();
        unzipFile.close();
        m_noErrors = false;
        return "";
    }

    m_FillCurrentZipExceptFile(unzipFile, fileName);

    m_Close();
    return m_zipCopyPath;
}

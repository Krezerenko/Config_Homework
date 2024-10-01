

#include "Unzipper.h"

#include <iostream>
#include <QFile>

ZipHandler::Unzipper::Unzipper(const QString &zipPath)
{
    m_currentZip = nullptr;
    m_zipIsOpen = false;
    m_zipFileIsOpen = false;
    m_noErrors = true;
    m_unzipPath = "";
    zipFile tempZip = zipOpen(zipPath.toStdString().c_str(), APPEND_STATUS_ADDINZIP);
    if (tempZip == nullptr)
    {
        std::cerr << "Failed to locate archive on path.\n";
        m_noErrors = false;
        return;
    }
    m_zipPath = zipPath;
    zipClose(tempZip, nullptr);
}

ZipHandler::Unzipper::Unzipper()
{
    m_currentZip = nullptr;
    m_zipIsOpen = false;
    m_zipFileIsOpen = false;
    m_noErrors = false;
    m_unzipPath = "";
    m_zipPath = "";
}

ZipHandler::Unzipper::~Unzipper()
{
    m_Close();
    RemoveFiles();
}

bool ZipHandler::Unzipper::m_Open(const QString &zipPath)
{
    if (m_zipIsOpen && zipPath == m_zipPath) return true;

    m_noErrors = true;
    m_currentZip = unzOpen(zipPath.toStdString().c_str());
    if (m_currentZip == nullptr)
    {
        std::cerr << "Failed to open archive for reading.\n";
        m_noErrors = false;
        m_zipIsOpen = false;
        m_zipPath = "";
        return false;
    }
    m_zipIsOpen = true;
    m_zipPath = zipPath;
    return true;
}

bool ZipHandler::Unzipper::m_Close()
{
    int errorCode;
    if (!m_zipIsOpen) return true;
    m_CloseCurrentFileNoRec();
    errorCode = unzClose(m_currentZip);
    if (errorCode != UNZ_OK)
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

bool ZipHandler::Unzipper::m_GoToFirstFile()
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to locate first file in archive: Archive is not open for reading.\n";
        return false;
    }
    if (m_zipFileIsOpen)
    {
        std::cerr << "Failed to locate first file in archive: Close current file first.\n";
        return false;
    }

    int errorCode = unzGoToFirstFile(m_currentZip);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to locate first file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    return true;
}

bool ZipHandler::Unzipper::m_GoToNextFile()
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to locate next file in archive: Archive is not open for reading.\n";
        return false;
    }
    if (m_zipFileIsOpen)
    {
        std::cerr << "Failed to locate next file in archive: Close current file first.\n";
        return false;
    }

    int errorCode = unzGoToNextFile(m_currentZip);
    if (errorCode == UNZ_END_OF_LIST_OF_FILE) return false;
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to locate next file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    return true;
}

bool ZipHandler::Unzipper::m_GetCurrentFileInfo(unz_file_info *info, char *nameBuffer, unsigned int nameBufferSize)
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to read info for file in archive: Archive is not open for reading.\n";
        return false;
    }

    int errorCode = unzGetCurrentFileInfo(m_currentZip, info, nameBuffer, nameBufferSize, nullptr, 0, nullptr, 0);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to read info for file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    return true;
}

bool ZipHandler::Unzipper::m_OpenCurrentFile()
{
    if (m_zipFileIsOpen && m_zipIsOpen) return true;
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to open file in archive: Archive is not open for reading.\n";
        return false;
    }
    int errorCode = unzOpenCurrentFile(m_currentZip);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to open file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = true;
    return true;
}

bool ZipHandler::Unzipper::m_CloseCurrentFileNoRec()
{
    if (!m_zipFileIsOpen) return true;
    int errorCode = unzCloseCurrentFile(m_currentZip);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to close file in archive.\n";
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = false;
    return true;
}

bool ZipHandler::Unzipper::m_CloseCurrentFile()
{
    if (!m_zipFileIsOpen) return true;
    if (!m_zipIsOpen) return true;
    int errorCode = unzCloseCurrentFile(m_currentZip);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to close file in archive.\n";
        m_Close();
        m_noErrors = false;
        return false;
    }
    m_zipFileIsOpen = false;
    return true;
}

int ZipHandler::Unzipper::m_ReadCurrentFile(char *fileBuffer, unsigned int fileBufferSize)
{
    if (!m_noErrors) return -1;
    if (!m_zipIsOpen)
    {
        std::cerr << "Failed to read file in archive: Archive is not open for reading.\n";
        return -1;
    }
    if (!m_zipFileIsOpen)
    {
        std::cerr << "Failed to read file in archive: File is not open.\n";
        return -1;
    }

    int filePortionSize = unzReadCurrentFile(m_currentZip, fileBuffer, fileBufferSize);
    if (filePortionSize < 0)
    {
        std::cerr << filePortionSize << ": Failed to read file in archive.\n";
        m_Close();
        m_noErrors = false;
        return -1;
    }
    return filePortionSize;
}

void ZipHandler::Unzipper::RemoveFiles()
{
    if (!m_unzipPath.isEmpty())
    {
        m_Close();
        QFile file(m_unzipPath);
        file.remove();
        m_unzipPath = "";
    }
}

QString ZipHandler::Unzipper::Unzip()
{
    if (!m_noErrors) return "";

    m_Open(m_zipPath);
    if (m_unzipPath.isEmpty())
    {
        m_unzipPath = m_GenerateTempFileName();
    }
    QFile unzipFile(m_unzipPath);
    if (!unzipFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        std::cerr << "Failed to create a working file.\n";
        m_Close();
        unzipFile.close();
        m_noErrors = false;
        return "";
    }

    constexpr unsigned int nameBufferSize = 1024;
    constexpr unsigned int fileBufferSize = 1024 * 8;
    char nameBuffer[nameBufferSize];
    char fileBuffer[fileBufferSize];
    unsigned int nameSize;
    int filePortionSize;
    unsigned long fileSize;

    if (!m_GoToFirstFile())
    {
        unzipFile.close();
        return "";
    }
    unz_file_info fileInfo;
    do
    {
        if (!m_GetCurrentFileInfo(&fileInfo, nameBuffer, nameBufferSize))
        {
            unzipFile.close();
            return "";
        }
        nameSize = fileInfo.size_filename;
        fileSize = fileInfo.uncompressed_size;
        unzipFile.write(reinterpret_cast<char*>(&nameSize), NameLenSize);
        unzipFile.write(nameBuffer, nameSize);

        if (!m_OpenCurrentFile())
        {
            unzipFile.close();
            return "";
        }

        unzipFile.write(reinterpret_cast<char*>(&fileSize), FileLenSize);
        do
        {
            filePortionSize = m_ReadCurrentFile(fileBuffer, fileBufferSize);
            if (filePortionSize < 0)
            {
                unzipFile.close();
                return "";
            }
            unzipFile.write(fileBuffer, filePortionSize);
        }
        while(filePortionSize > 0);

        m_CloseCurrentFile();
    }
    while (m_GoToNextFile());
    m_Close();
    unzipFile.close();
    return unzipFile.fileName();
}

bool ZipHandler::Unzipper::FindFile(const QString &path)
{
    if (!m_noErrors) return false;
    if (!m_zipIsOpen)
    {
        m_Open(m_zipPath);
    }

    constexpr unsigned int nameBufferSize = 1024;
    char nameBuffer[nameBufferSize];

    if (!m_GoToFirstFile()) return false;
    unz_file_info fileInfo;
    do
    {
        if (!m_GetCurrentFileInfo(&fileInfo, nameBuffer, nameBufferSize))
        {
            return false;
        }
        if (QString(nameBuffer) == path)
        {
            return true;
        }
    }
    while (m_GoToNextFile());
    m_Close();
    return false;
}

bool ZipHandler::Unzipper::GetFilesInDirectory(const QString &directoryPath, std::vector<QString> &buffer)
{
    if (!m_noErrors) return false;
    if (!m_Open(m_zipPath)) return false;

    constexpr unsigned int nameBufferSize = 1024;
    char nameBuffer[nameBufferSize];

    if (!m_GoToFirstFile()) return false;
    unz_file_info fileInfo;
    QString name;
    do
    {
        if (!m_GetCurrentFileInfo(&fileInfo, nameBuffer, nameBufferSize))
        {
            return false;
        }
        name = QString(nameBuffer);
        if (!name.startsWith(directoryPath)) continue;
        if (name == directoryPath) continue;

        name.replace(directoryPath, "");
        if (name.back() == '/')
        {
            name.removeLast();
        }
        if (name.contains('/')) continue;
        buffer.push_back(name);
    }
    while (m_GoToNextFile());
    m_Close();
    return true;
}

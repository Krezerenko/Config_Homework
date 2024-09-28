

#include "ZipHandler.h"
#include <string>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#include <iostream>
#include <fstream>
#include <random>
#include <QString>
#include <QFile>
#include <QDataStream>

ZipHandler::ZipHandler(const QString &zipPath)
{
    m_noErrors = true;
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

ZipHandler::~ZipHandler()
{
    if (!m_unzipPath.isEmpty())
    {
        QFile file(m_unzipPath);
        file.remove();
    }
    if (!m_zipCopyPath.isEmpty())
    {
        QFile file(m_zipCopyPath);
        file.remove();
    }
}

QString ZipHandler::m_GenerateTempFileName()
{
    unsigned int len = 32;
    QString alphabet = "0123456789-_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned int alphabetSize = alphabet.size();
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, alphabetSize - 1);
    QString name(len, ' ');
    for (QChar &c : name)
    {
        c = alphabet[dist(rng)];
    }
    name = '.' + name;
    return name;
}

QString ZipHandler::m_Unzip()
{
    if (!m_noErrors) return "";
    int errorCode;
    unzFile zipRead = unzOpen(m_zipPath.toStdString().c_str());
    if (zipRead == nullptr)
    {
        std::cerr << "Failed to open archive for reading.\n";
        unzClose(zipRead);
        m_noErrors = false;
        return "";
    }
    m_unzipPath = m_GenerateTempFileName();
    QFile tempFile(m_unzipPath);
    if (!tempFile.open(QIODevice::WriteOnly))
    {
        std::cerr << "Failed to create a working file.\n";
        unzClose(zipRead);
        tempFile.close();
        m_noErrors = false;
        return "";
    }

    constexpr unsigned int nameBufferSize = 1024;
    constexpr unsigned int fileBufferSize = 1024 * 8;
    unsigned int nameSize;
    int filePortionSize;
    unsigned long fileSize;
    char nameBuffer[nameBufferSize];
    char fileBuffer[fileBufferSize];

    errorCode = unzGoToFirstFile(zipRead);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to locate first file in archive.\n";
        unzClose(zipRead);
        tempFile.close();
        m_noErrors = false;
        return "";
    }
    unz_file_info fileInfo;
    do
    {
        errorCode = unzGetCurrentFileInfo(zipRead, &fileInfo, nameBuffer, nameBufferSize, nullptr, 0, nullptr, 0);
        if (errorCode != UNZ_OK)
        {
            std::cerr << errorCode << ": Failed to read info for file in archive.\n";
            unzClose(zipRead);
            tempFile.close();
            m_noErrors = false;
            return "";
        }
        nameSize = fileInfo.size_filename;
        fileSize = fileInfo.uncompressed_size;
        tempFile.write(reinterpret_cast<char*>(&nameSize), NameLenSize);
        tempFile.write(nameBuffer, nameSize);

        errorCode = unzOpenCurrentFile(zipRead);
        if (errorCode != UNZ_OK)
        {
            std::cerr << errorCode << ": Failed to open file in archive.\n";
            unzCloseCurrentFile(zipRead);
            unzClose(zipRead);
            tempFile.close();
            m_noErrors = false;
            return "";
        }

        tempFile.write(reinterpret_cast<char*>(&fileSize), FileLenSize);
        do
        {
            filePortionSize = unzReadCurrentFile(zipRead, fileBuffer, fileBufferSize);
            if (filePortionSize < 0)
            {
                std::cerr << filePortionSize << ": Failed to read file in archive.\n";
                unzCloseCurrentFile(zipRead);
                unzClose(zipRead);
                tempFile.close();
                m_noErrors = false;
                return "";
            }
            tempFile.write(fileBuffer, filePortionSize);
        }
        while(filePortionSize > 0);

        unzCloseCurrentFile(zipRead);
    }
    while (unzGoToNextFile(zipRead) == UNZ_OK);
    unzClose(zipRead);
    tempFile.close();
    return tempFile.fileName();
}

bool ZipHandler::m_ApplyChanges()
{
    if (!m_noErrors) return false;
    return false;
}

bool ZipHandler::FindFile(const QString &path)
{
    if (!m_noErrors) return false;
    int errorCode;

    zipFile zipRead = unzOpen(m_zipPath.toStdString().c_str());

    constexpr unsigned int nameBufferSize = 1024;
    char nameBuffer[nameBufferSize];

    errorCode = unzGoToFirstFile(zipRead);
    if (errorCode != UNZ_OK)
    {
        std::cerr << errorCode << ": Failed to locate first file in archive.\n";
        unzClose(zipRead);
        m_noErrors = false;
        return false;
    }
    unz_file_info fileInfo;
    do
    {
        errorCode = unzGetCurrentFileInfo(zipRead, &fileInfo, nameBuffer, nameBufferSize, nullptr, 0, nullptr, 0);
        if (errorCode != UNZ_OK)
        {
            std::cerr << errorCode << ": Failed to read info for file in archive.\n";
            unzClose(zipRead);
            m_noErrors = false;
            return false;
        }
        if (QString(nameBuffer) == path)
        {
            return true;
        }
    }
    while (unzGoToNextFile(zipRead) == UNZ_OK);
    unzClose(zipRead);
    return false;
}

bool ZipHandler::WriteToFile()
{
    if (!m_noErrors) return false;
    return false;
}

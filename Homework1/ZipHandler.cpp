

#include "ZipHandler.h"
#include "Unzipper.h"
#include "Zipper.h"

#include <string>
#include <random>
#include <QString>
#include <iostream>

ZipHandler::ZipHandler(const QString &zipPath)
{
    m_noErrors = true;
    m_pUnzipper = new Unzipper(zipPath);
    m_pZipper = new Zipper();
    OpenArchive(zipPath);
}

//ZipHandler::ZipHandler()
//{
//    m_noErrors = true;
//    m_pUnzipper = new Unzipper();
//    m_pZipper = new Zipper();
//}

ZipHandler::~ZipHandler()
{
    delete m_pUnzipper;
    delete m_pZipper;
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

void ZipHandler::m_ReplaceOriginal()
{
    QFile file1(m_zipCopyPath);
    QFile file2(m_zipPath);

    constexpr unsigned int fileBufferSize = 1024 * 8;
    unsigned int filePortionSize;
    char fileBuffer[fileBufferSize];

    file1.open(QIODevice::ReadOnly);
    file2.open(QIODevice::WriteOnly);

    while(!file1.atEnd())
    {
        filePortionSize = file1.read(fileBuffer, fileBufferSize);
        file2.write(fileBuffer, filePortionSize);
    }

    file1.close();
    file2.close();
    m_RemoveFiles();
}

void ZipHandler::m_RemoveFiles()
{
    m_pUnzipper->RemoveFiles();
    m_pZipper->RemoveFiles();
    m_zipCopyPath = "";
    m_unzipPath = "";
}

bool ZipHandler::OpenArchive(const QString &zipPath)
{
    m_noErrors = true;
    QFile tempFile(zipPath);
    if (!tempFile.exists())
    {
        std::cerr << "Zip archive doesn't exist on the given path.\n";
        return false;
    }
    m_zipPath = zipPath;
    tempFile.close();
    return true;
}

bool ZipHandler::FindFile(const QString &path)
{
    return m_pUnzipper->FindFile(path);
}

bool ZipHandler::GetFilesInDirectory(const QString &directoryPath, std::vector<QString> &buffer)
{
    return m_pUnzipper->GetFilesInDirectory(directoryPath, buffer);
}

bool ZipHandler::WriteToFile(const QString &fileName, const QString &data)
{
    m_unzipPath = m_pUnzipper->Unzip();
    m_noErrors = m_unzipPath != "";
    if (m_pUnzipper->FindFile(fileName))
    {
        m_zipCopyPath = m_pZipper->ZipWriteToFileRaw(m_unzipPath, fileName, data.toStdString().data(), data.size());
    }
    else
    {
        m_zipCopyPath = m_pZipper->ZipAddNewFileRaw(m_unzipPath, fileName, data.toStdString().data(), data.size());
    }
    m_noErrors = m_zipCopyPath != "";
    m_ReplaceOriginal();
    return m_noErrors;
}

bool ZipHandler::AddFile(const QString &fileName)
{
    m_unzipPath = m_pUnzipper->Unzip();
    m_noErrors = m_unzipPath != "";
    if (m_pUnzipper->FindFile(fileName)) return true;

    m_zipCopyPath = m_pZipper->ZipAddNewFileRaw(m_unzipPath, fileName, nullptr, 0);
    m_noErrors = m_zipCopyPath != "";
    m_ReplaceOriginal();
    return m_noErrors;
}

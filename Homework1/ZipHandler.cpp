

#include "ZipHandler.h"
#include "Unzipper.h"
#include "Zipper.h"

#include <string>
#include <random>
#include <QString>

ZipHandler::ZipHandler(const QString &zipPath)
{
    m_noErrors = true;
    m_pUnzipper = new Unzipper(zipPath);
    m_pZipper = new Zipper();
}

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

bool ZipHandler::FindFile(const QString &path)
{
    return m_pUnzipper->FindFile(path);
}

bool ZipHandler::WriteToFile()
{
    m_unzipPath = m_pUnzipper->Unzip();
    m_noErrors = m_unzipPath != "";
    m_zipCopyPath = m_pZipper->ZipNoChange(m_unzipPath);
    m_noErrors = m_zipCopyPath != "";
    return m_noErrors;
}

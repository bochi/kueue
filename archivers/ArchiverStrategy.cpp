/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ArchiverStrategy.h"
#include "Utility.h"

#include <QTextStream>
#include <QString>
#include <QFile>

ArchiverStrategy::ArchiverStrategy(const QString &name, const FileSignature &sig)
    : name(name)
    , supported(false)
    , signature(sig)
{
}

ArchiverStrategy::~ArchiverStrategy()
{
}

void ArchiverStrategy::setSupported(bool f)
{
    supported = f;
}

void ArchiverStrategy::addExtension(const QString &ext)
{
    if (extensions.indexOf(ext) < 0)
    {
        extensions.append(ext);
    }
}

void ArchiverStrategy::setFileMagic(const FileSignature &sig)
{
    signature = sig;
}

QStringList ArchiverStrategy::fillTemplateArguments(const QStringList & inargs, const QString &filename)
{
    QStringList args;
    foreach (QString s, inargs)
    {
        if (s == "@F")
        {
            args << filename;
        }
        else
        {
            args << s;
        }
    }
    return args;
}

void ArchiverStrategy::setExtractArguments(const QString &command)
{
    extractArgs = command.split(" ", QString::SkipEmptyParts);
}

void ArchiverStrategy::setListArguments(const QString &command)
{
    listArgs = command.split(" ", QString::SkipEmptyParts);
}

void ArchiverStrategy::setExecutables(const QString &exec1, const QString &exec2)
{
    executables.clear();
    executables.append(exec1);
    if (exec2 != QString::null)
    {
        executables.append(exec2);
    }
}

QStringList ArchiverStrategy::getExtractArguments(const QString &filename) const
{
    return fillTemplateArguments(extractArgs, filename);
}

QStringList ArchiverStrategy::getExtractArguments() const
{
    return extractArgs;
}

QStringList ArchiverStrategy::getListArguments(const QString &filename) const
{
    return fillTemplateArguments(listArgs, filename);
}

QStringList ArchiverStrategy::getListArguments() const
{
    return listArgs;
}

QStringList ArchiverStrategy::getExtensions() const
{
    return extensions;
}

bool ArchiverStrategy::canOpen(QFile *f) const
{
    return supported && signature.matches(f);
}

bool ArchiverStrategy::canOpen(const QString &filename) const
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        return canOpen(&file);
    }
	return false;
}

QList<ArchiverHint> ArchiverStrategy::getHints() const
{
    return QList<ArchiverHint>(); // no hints by default
}

bool ArchiverStrategy::isSupported() const
{
    return supported;
}

ArchiverStrategy::operator ArchiverStatus() const
{
    Q_ASSERT(executables.size() > 0);
    return ArchiverStatus(supported, name, extensions, executables);
}

FileSignature::FileSignature(unsigned int offset, const char *pattern, unsigned int len)
    : offset(offset)
{
    this->pattern = QByteArray(pattern, len);
}

FileSignature::FileSignature()
    : offset(0)
{
}

FileSignature::FileSignature(const FileSignature &sig)
{
    pattern = sig.pattern;
    offset = sig.offset;
}

FileSignature::~FileSignature()
{
}

bool FileSignature::matches(QFile *file) const
{
    if (pattern.size() > 0 && file->seek(offset))
    {
        for (int i=0; i<pattern.size(); i++)
        {
            char c;
            if (!file->getChar(&c))
                return false;
            if (c != pattern.at(i))
                return false;
        }
        return true;
    }
    return false;
}

FileSignature& FileSignature::operator =(const FileSignature &sig)
{
    if (this != &sig)
    {
        pattern = sig.pattern;
        offset = sig.offset;
    }
    return *this;
}

using Utility::which;

AceArchiverStrategy::AceArchiverStrategy()
    : ArchiverStrategy("ace", FileSignature(8, "\x2a\x41\x43\x45", 4))
{
}

AceArchiverStrategy::~AceArchiverStrategy()
{
}

void AceArchiverStrategy::configure()
{
    addExtension(".ace");
    addExtension(".cba");
    setExecutables("unace");

    if (which("unace") != QString::null)
    {
        setExtractArguments("unace x -y -c- @F");
        setListArguments("unace l -y -c- @F");
        setSupported();
    }
}

P7zipArchiverStrategy::P7zipArchiverStrategy()
    : ArchiverStrategy("p7zip", FileSignature())
{
}

P7zipArchiverStrategy::~P7zipArchiverStrategy()
{
}

void P7zipArchiverStrategy::configure()
{
    addExtension(".7z");
    addExtension(".cb7");

    setExecutables("7z", "7zr");

    if (which("7z") != QString::null)
    {
        setExtractArguments("7z x @F");
        setListArguments("7z l @F");
        setSupported();
    }
    else if (which("7zr") != QString::null)
    {
        setExtractArguments("7zr x @F");
        setListArguments("7zr l @F");
        setSupported();
    }
}

RarArchiverStrategy::RarArchiverStrategy()
    : ArchiverStrategy("rar", FileSignature(0, "\x52\x61\x72\x21", 4)), nonfree_unrar(false)
{
}

RarArchiverStrategy::~RarArchiverStrategy()
{
}

void RarArchiverStrategy::configure()
{
    addExtension(".rar");
    addExtension(".cbr");
    setExecutables("rar", "unrar");

    if (which("rar") != QString::null)
    {
        setExtractArguments("rar -o+ x @F");
        setListArguments("rar lb @F");
        setSupported();
    }
    else if (which("unrar") != QString::null)
    {
        FILE *f;
        //
        // now determine which unrar it is - free or non-free
        if ((f = popen("unrar", "r")) != NULL)
        {
            QRegExp regexp("^UNRAR.+freeware");
            for (QTextStream s(f); !s.atEnd(); )
            {
                if (regexp.indexIn(s.readLine()) >= 0)
                {
                    nonfree_unrar = true;
                    break;
                }
            }
            pclose(f);
            if (nonfree_unrar)
            {
                setExtractArguments("unrar -o+ x @F");
                setListArguments("unrar lb @F");
            }
            else
            {
                setExtractArguments("unrar -o+ -x @F");
                setListArguments("unrar -t @F");
            }
            setSupported();
        }
    }
    else if (which("unrar-free") != QString::null) //some distros rename free unrar like this
    {
        setExtractArguments("unrar-free -o+ -x @F");
        setListArguments("unrar-free -t @F");
        setSupported();
    }
}

QList<ArchiverHint> RarArchiverStrategy::getHints() const
{
    QList<ArchiverHint> hints;
    if (isSupported() && !nonfree_unrar)
    {
        hints.append(ArchiverHint(
                         ArchiversConfiguration::tr("Free (opensource) version of unrar was detected. "
                                                    "This version has problems with many rar archives. "
                                                    "It is recommended to install the non-free unrar and restart QComicBook."),
                         ArchiverHint::Warning));
    }
    return hints;
}

TarArchiverStrategy::TarArchiverStrategy()
    : ArchiverStrategy("tar", FileSignature())
{
}

TarArchiverStrategy::~TarArchiverStrategy()
{
}

void TarArchiverStrategy::configure()
{
    addExtension(".tar");
    setExecutables("tar");

    if (which("tar") != QString::null)
    {
        setExtractArguments("tar --overwrite -xvf @F");
        setListArguments("tar -tf @F");
        setSupported();
    }
}

Tarbz2ArchiverStrategy::Tarbz2ArchiverStrategy()
    : ArchiverStrategy("tar.bz2", FileSignature())
{
}

Tarbz2ArchiverStrategy::~Tarbz2ArchiverStrategy()
{
}

void Tarbz2ArchiverStrategy::configure()
{
    addExtension(".tbz");
    addExtension(".tar.bz2");
    addExtension(".cbb");
    setExecutables("tar");

    if (which("tar") != QString::null)
    {
        setExtractArguments("tar --overwrite -xvjf @F");
        setListArguments("tar -tjf @F");
        setSupported();
    }
}

TargzArchiverStrategy::TargzArchiverStrategy()
    : ArchiverStrategy("tar.gz", FileSignature())
{
}

TargzArchiverStrategy::~TargzArchiverStrategy()
{
}

void TargzArchiverStrategy::configure()
{
    addExtension(".tar.gz");
    addExtension(".tgz");
    addExtension(".cbg");
    setExecutables("tar");

    if (which("tar") != QString::null)
    {
        setExtractArguments("tar --overwrite -xvzf @F");
        setListArguments("tar -tzf @F");
        setSupported();
    }
}

ZipArchiverStrategy::ZipArchiverStrategy()
    : ArchiverStrategy("zip", FileSignature(0, "\x50\x4b\x03\x04", 4))
{
}

ZipArchiverStrategy::~ZipArchiverStrategy()
{
}

void ZipArchiverStrategy::configure()
{
    addExtension(".zip");
    addExtension(".cbz");
    addExtension(".cbr");
    setExecutables("unzip");

    if (which("unzip") != QString::null)
    {
        setExtractArguments("unzip -o @F");
        setListArguments("unzip -l @F");
        setSupported();
    }
}

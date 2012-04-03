/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
            Thanks to QComicBook for the inspiration :-)
     Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the

    Free Software Foundation, Inc.
    59 Temple Place - Suite 330
    Boston, MA  02111-1307, USA

    Have a lot of fun :-)
*/

#include "ArchiversConfiguration.h"
#include "ArchiverStrategy.h"

#include <QFile>

ArchiverHint::ArchiverHint(const QString &message, Severity severity)
{
    msg = message;
    sev = severity;
}

ArchiverHint::~ArchiverHint()
{
}

ArchiverStatus::ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QString &exec1, const QString &exec2)
    : m_name(name),
      m_supported(supported),
      m_extensions(extensions)
{
    m_executables.append(exec1);
    if (exec2 != QString::null)
    {
        m_executables.append(exec2);
    }
}

ArchiverStatus::ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QStringList &execlist)
    : m_name(name),
      m_supported(supported),
      m_executables(execlist),
      m_extensions(extensions)
{
}

ArchiverStatus::~ArchiverStatus()
{
}

bool ArchiverStatus::isSupported() const
{
    return m_supported;
}

QString ArchiverStatus::name() const
{
    return m_name;
}

QStringList ArchiverStatus::executables() const
{
    return m_executables;
}

QStringList ArchiverStatus::extensions() const
{
    return m_extensions;
}

ArchiversConfiguration& ArchiversConfiguration::instance()
{
    static ArchiversConfiguration cfg;
    return cfg;
}

ArchiversConfiguration::ArchiversConfiguration()
{
    archivers.append(new ZipArchiverStrategy());
    archivers.append(new RarArchiverStrategy());
    archivers.append(new AceArchiverStrategy());
    archivers.append(new TargzArchiverStrategy());
    archivers.append(new TarArchiverStrategy());
    archivers.append(new Tarbz2ArchiverStrategy());
    archivers.append(new P7zipArchiverStrategy());

    foreach (ArchiverStrategy *s, archivers)
    {
        s->configure();
    }
}

ArchiversConfiguration::~ArchiversConfiguration()
{
    foreach (ArchiverStrategy *s, archivers)
    {
        delete s;
    }
}

ArchiverStrategy* ArchiversConfiguration::findStrategy(const QString &filename) const
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        foreach (ArchiverStrategy *s, archivers)
        {
            if (s->canOpen(&file))
            {
                file.close();
                return s;
            }
        }
        file.close();
        foreach (ArchiverStrategy *s, archivers)
        {
            foreach (QString ext, s->getExtensions())
            {
                if (filename.endsWith(ext, Qt::CaseInsensitive))
                {
                    return s;
                }
            }
        }
    }
    return NULL;
}

void ArchiversConfiguration::getExtractArguments(const QString &filename, QStringList &extract, QStringList &list) const
{
    ArchiverStrategy *s = findStrategy(filename);
    if (s)
    {
        extract = s->getExtractArguments(filename);
        list = s->getListArguments(filename);
    }
    else
    {
        extract.clear();
        list.clear();
    }
}

QStringList ArchiversConfiguration::getExtractArguments(const QString &filename) const
{
    ArchiverStrategy *s = findStrategy(filename);
    if (s)
    {
        return s->getExtractArguments(filename);
    }
    return QStringList();
}

QStringList ArchiversConfiguration::getListArguments(const QString &filename) const
{
    ArchiverStrategy *s = findStrategy(filename);
    if (s)
    {
        return s->getListArguments(filename);
    }
    return QStringList();
}

QStringList ArchiversConfiguration::supportedOpenExtensions() const
{
    QStringList extlist;
    foreach (ArchiverStrategy *s, archivers)
    {
        foreach (const QString ext, s->getExtensions())
        {
            extlist.append("*" + ext);
        }
    }
    return extlist;
}

QList<ArchiverStatus> ArchiversConfiguration::getArchiversStatus() const
{
    QList<ArchiverStatus> status;
    foreach (ArchiverStrategy *s, archivers)
    {
        status.append(ArchiverStatus(*s));

    }
    return status;

}

QList<ArchiverHint> ArchiversConfiguration::getHints() const
{
    QList<ArchiverHint> hints;
    bool all_supported(true);
    foreach (ArchiverStrategy *s, archivers)
    {
        hints.append(s->getHints());
        all_supported = all_supported && s->isSupported();
    }
    if (!all_supported)
    {
        hints.append(ArchiverHint(tr("Some archives are not supported because essential external utilities are missing. "
                       "Please install required executables and restart QComicBook. "
                       "Note: if more than one executable is listed for given archive type, then it is sufficient to install "
                       " just one of them."), ArchiverHint::Error));
    }
    return hints;
}

#include "ArchiversConfiguration.moc"

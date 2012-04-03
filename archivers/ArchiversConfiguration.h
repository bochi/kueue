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

#ifndef __ARCHIVERS_CONFIGURATION_H
#define __ARCHIVERS_CONFIGURATION_H

#include <QObject>
#include <QList>
#include <QStringList>

    class ArchiverStrategy;

    class ArchiverHint
    {
    public:
        enum Severity { Info, Warning, Error };

        ArchiverHint(const QString &message, Severity severity=Info);
        ~ArchiverHint();

        Severity getSeverity() const
        {
            return sev;
        }

        QString getMessage() const
        {
            return msg;
        }

    private:
        QString msg;
        Severity sev;
    };
    
    class ArchiverStatus
    {
    public:
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QString &exec1, const QString &exec2=QString::null);
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QStringList &execlist);
        ~ArchiverStatus();

        bool isSupported() const;
        QString name() const;
        QStringList executables() const;
        QStringList extensions() const;

    private:
        QString m_name;
        QStringList m_executables;
        QStringList m_extensions;
        bool m_supported;
    };

    class ArchiversConfiguration: public QObject
    {
    Q_OBJECT

    public:
        static ArchiversConfiguration& instance();
        void getExtractArguments(const QString &filename, QStringList &extract, QStringList &list) const;
        QStringList getExtractArguments(const QString &filename) const;
        QStringList getListArguments(const QString &filename) const;
        QStringList supportedOpenExtensions() const;
        QList<ArchiverStatus> getArchiversStatus() const;
        QList<ArchiverHint> getHints() const;

    private:
        ArchiversConfiguration();
        ~ArchiversConfiguration();
        ArchiverStrategy* findStrategy(const QString &filename) const;

        QList<ArchiverStrategy *> archivers;
    };


#endif

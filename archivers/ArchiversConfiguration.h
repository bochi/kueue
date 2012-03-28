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

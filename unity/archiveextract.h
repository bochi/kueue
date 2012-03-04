/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>
             
            Thanks to the ark Team for the inspiration :-)
    
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

#ifndef ARCHIVEEXTRACT_H
#define ARCHIVEEXTRACT_H

#include <QDir>

#include "kueuethreads.h"
#include "config.h" 

class ArchiveExtract: public ThreadWeaver::Job 
{
    Q_OBJECT

    public:
        ArchiveExtract( const QString&, const QString& );
        ~ArchiveExtract();

        bool list();
        bool copyFiles();
        QString finalDir() { return mFinalDir; }

    private:
        void copyData(struct archive *source, struct archive *dest, bool partialprogress = true);

        #ifndef IS_WIN32
	struct ArchiveReadCustomDeleter;
        struct ArchiveWriteCustomDeleter;
#endif
        typedef QScopedPointer<struct archive, ArchiveReadCustomDeleter> ArchiveRead;
        typedef QScopedPointer<struct archive, ArchiveWriteCustomDeleter> ArchiveWrite;

        int m_cachedArchiveEntryCount;
        qlonglong m_currentExtractedFilesSize;
        
        qlonglong m_extractedFilesSize;
        QStringList m_writtenFiles;
        QString mFileName;
        QString mDestination;
        QString mFinalDir;
        ArchiveRead m_archiveReadDisk;
    
    protected: 
        void run();        
    
    signals:
        void extracted( const QString&, const QString& );
};

#endif

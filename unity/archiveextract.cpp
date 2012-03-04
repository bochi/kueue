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

#include "archiveextract.h"

#ifndef IS_WIN32
#include <archive.h>
#include <archive_entry.h>
#endif
#include <sys/stat.h>

#include <QDebug>

struct ArchiveExtract::ArchiveReadCustomDeleter
{
    #ifndef IS_WIN32
    static inline void cleanup( struct archive *a )
    {
        if ( a ) 
        {
            archive_read_finish( a );
        }
    }
    #endif
};

struct ArchiveExtract::ArchiveWriteCustomDeleter
{
    #ifndef IS_WIN32
    static inline void cleanup( struct archive *a )
    {
        if ( a ) 
        {
            archive_write_finish( a );
        }
    }
    #endif
};

ArchiveExtract::ArchiveExtract( const QString& file, const QString& dest ) : ThreadWeaver::Job()
    , mFileName( file )
    , mDestination( dest )
    , m_cachedArchiveEntryCount( 0 )
    , m_extractedFilesSize( 0 )
    #ifndef IS_WIN32
    , m_archiveReadDisk( archive_read_disk_new() )
    #endif
    
{
    qDebug() << "[ARCHIVEEXTRACT] Constructing";
}

ArchiveExtract::~ArchiveExtract()
{
    qDebug() << "[ARCHIVEEXTRACT] Destroying";
}

void ArchiveExtract::run()
{
    #ifndef IS_WIN32
    archive_read_disk_set_standard_lookup( m_archiveReadDisk.data() );
    copyFiles();
    #endif
}

bool ArchiveExtract::list()
{
    #ifndef IS_WIN32
    ArchiveRead arch_reader( archive_read_new() );

    if ( !( arch_reader.data() ) ) 
    {
        return false;
    }

    if ( archive_read_support_compression_all( arch_reader.data() ) != ARCHIVE_OK )
    {
        return false;
    }

    if ( archive_read_support_format_all( arch_reader.data() ) != ARCHIVE_OK ) 
    {
        return false;
    }

    if ( archive_read_open_filename( arch_reader.data(), QFile::encodeName( mFileName ), 10240 ) != ARCHIVE_OK ) 
    {
        qDebug() << "[ARCHIVEEXTRACT]" << mFileName << "Format not supported";
        return false;
    }

    m_cachedArchiveEntryCount = 0;
    m_extractedFilesSize = 0;

    struct archive_entry *aentry;
    int result;

    while ( ( result = archive_read_next_header( arch_reader.data(), &aentry ) ) == ARCHIVE_OK ) 
    {
        m_extractedFilesSize += ( qlonglong )archive_entry_size( aentry );
        emit jobStarted( "Extracting...", m_extractedFilesSize );
        m_cachedArchiveEntryCount++;
        archive_read_data_skip( arch_reader.data() );
    }

    if ( result != ARCHIVE_EOF ) 
    {
        qDebug() << "[ARCHIVEEXTRACT] Open failed" << QLatin1String( archive_error_string( arch_reader.data() ) );
        return false;
    }

    return archive_read_close( arch_reader.data() ) == ARCHIVE_OK;
    #endif
}

bool ArchiveExtract::copyFiles()
{
    #ifndef IS_WIN32
    qDebug() << "[ARCHIVEEXTRACT] Extracting" << mFileName.remove( mDestination + "/" ) << "to" << mDestination;
    QDir::setCurrent( mDestination );

    ArchiveRead arch( archive_read_new() );

    if ( !( arch.data() ) ) 
    {
        return false;
    }

    if ( archive_read_support_compression_all( arch.data() ) != ARCHIVE_OK ) 
    {
        return false;
    }

    if ( archive_read_support_format_all( arch.data() ) != ARCHIVE_OK ) 
    {
        return false;
    }

    if ( archive_read_open_filename( arch.data(), QFile::encodeName( mFileName ), 10240 ) != ARCHIVE_OK )
    {
        qDebug() << "[ARCHIVEEXTRACT]" << mFileName << "Format not supported";
        return false;
    }

    ArchiveWrite writer( archive_write_disk_new());
    
    if ( !( writer.data() ) ) 
    {
        return false;
    }

    archive_write_disk_set_options( writer.data(), 0 );

    int entryNr = 0;
    int totalCount = 0;
    
    list();
  
    totalCount = m_cachedArchiveEntryCount;
    
    m_currentExtractedFilesSize = 0;

    struct archive_entry *entry;
    
    while (archive_read_next_header(arch.data(), &entry) == ARCHIVE_OK) 
    {
        QString entryName = QDir::fromNativeSeparators( QFile::decodeName( archive_entry_pathname( entry ) ) );
        mFinalDir = QFileInfo( entryName ).dir().absolutePath();

        if ( entryName.startsWith(QLatin1Char( '/' ) ) ) 
        {
            qDebug() << "[ARCHIVEEXTRACT] Entries with absolute path found, not supported.";
            return false;
        }

        QFileInfo entryFI( entryName );
        const QString fileWithoutPath( entryFI.fileName() );
        const QString truncatedFilename( entryName.remove( mDestination + "/" ) );
        
        archive_entry_copy_pathname( entry, QFile::encodeName( truncatedFilename ).constData() );
        entryFI = QFileInfo( truncatedFilename );

        int header_response;
        
        if ( ( header_response = archive_write_header( writer.data(), entry ) ) == ARCHIVE_OK ) 
        {
            copyData( arch.data(), writer.data(), ( m_extractedFilesSize ) );

            if ( m_cachedArchiveEntryCount ) 
            {
                ++entryNr;
            }
            
            archive_entry_clear( entry );
        }
    }

    emit extracted( QFileInfo( mFileName ).fileName(), mFinalDir );
    qDebug() << "[ARCHIVEEXTRACT] Finished extracting" << mFinalDir;

    return archive_read_close( arch.data() ) == ARCHIVE_OK;
    #endif
}

void ArchiveExtract::copyData( struct archive *source, struct archive *dest, bool partialprogress )
{
    #ifndef IS_WIN32
    char buff[ 10240 ];
    ssize_t readBytes;

    readBytes = archive_read_data( source, buff, sizeof( buff ) );
    
    while ( readBytes > 0 ) 
    {
        archive_write_data( dest, buff, readBytes );
        
        if ( archive_errno( dest ) != ARCHIVE_OK ) 
        {
            qDebug() << "[ARCHIVEEXTRACT] Error while extracting" << archive_error_string( dest );
            return;
        }

        if ( partialprogress ) 
        {
            m_currentExtractedFilesSize += readBytes;
            emit jobProgress( m_currentExtractedFilesSize );
        }

        readBytes = archive_read_data( source, buff, sizeof( buff ) );
    }
    
    if ( m_currentExtractedFilesSize == m_extractedFilesSize ) 
    {
        emit done( this );
        emit jobFinished( this );
    }
    #endif
}

#include "archiveextract.moc"

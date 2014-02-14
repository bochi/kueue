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

#include "archiveextract.h"
#include "archivers/Utility.h"

#include <QDebug>
#include <QFile>
#include <magic.h>
#include <stdio.h>

using Utility::which;

ArchiveExtract::ArchiveExtract( const QString& archive, const QString& dest )
{
    qDebug() << "[ARCHIVEEXTRACT] Extracting" << archive << "to" << dest;
    
    mArchivePath = archive;
    mDestination = dest;
}

ArchiveExtract::~ArchiveExtract()
{
}

void ArchiveExtract::run()
{
    QFileInfo fi( mArchivePath );
    
    emit threadStarted( "Extracting...", 0 );
    
    QProcess info;
    QProcess extract;
    
    info.setProcessChannelMode( QProcess::MergedChannels );
    extract.setProcessChannelMode( QProcess::MergedChannels );

    QString app;
    QStringList extArgs;
    QStringList listArgs;
    
    QString mime = getMimeType( mArchivePath );
    
    if ( mime == "application/x-bzip2" )
    {
        if ( which( "tar" ) != QString::null )
        {
            app = "tar";
            #ifdef IS_WIN32
            extArgs <<  "-xvjf" << mArchivePath;
            #elif defined IS_OSX
            extArgs <<  "xjfv" << mArchivePath;
            #else
            extArgs <<  "--overwrite" << "-xvjf" << mArchivePath;
            #endif
            listArgs << "tjf" << mArchivePath;
        }
    }
    else if ( mime == "application/x-gzip" )
    {
        if ( ( mArchivePath.endsWith( ".gz" ) ) &&
            !( mArchivePath.endsWith( ".tar.gz" ) ) &&
             ( which( "gunzip" ) != QString::null ) )
        {
            app = "gunzip";
            
            extArgs <<  mArchivePath;
            listArgs << "-l" << mArchivePath;
        }
        else if ( which( "tar" ) != QString::null )
        {
            app = "tar";
            
            #ifdef IS_WIN32
            extArgs << "-xvzf" << mArchivePath;
            #elif defined IS_OSX
            extArgs << "xvzf" << mArchivePath;
            #else
            extArgs << "--overwrite" << "-xvzf" << mArchivePath;
            #endif
            listArgs << "tzf" << mArchivePath;
        }
    }
    else if ( mime == "application/x-rar" )
    {
        if ( which( "rar" ) != QString::null )
        {
            app = "rar";
            
            extArgs << "-o+" << "x" + mArchivePath;
            listArgs << "lb" << mArchivePath;
        }
        else if ( which( "unrar" ) != QString::null )
        {
            FILE* f;
            bool nonfree_unrar;
            
            if ( ( f = popen( "unrar", "r" ) ) != NULL )
            {
                QRegExp regexp( "^UNRAR.+freeware" );
                
                for ( QTextStream s( f ); !s.atEnd(); )
                {
                    if ( regexp.indexIn( s.readLine() ) >= 0 )
                    {
                        nonfree_unrar = true;
                        break;
                    }
                }
                
                pclose( f );
                
                if ( nonfree_unrar )
                {
                    app = "unrar";
                    
                    extArgs << "-o+" << "x" << mArchivePath;
                    listArgs << "lb" << mArchivePath;
                }
                else
                {
                    extArgs << "-o+" << "-x" << mArchivePath;
                    listArgs << "-t" << mArchivePath;
                }
            }
        }
        else if ( which( "unrar-free" ) != QString::null )
        {
            app = "unrar-free";
            
            extArgs <<  "-o+" << "-x" << mArchivePath;
            listArgs << "-free" << "-t" << mArchivePath;
        }
    }
    else if ( mime == "application/zip" )
    {
        if ( which( "unzip" ) != QString::null )
        {
            app = "unzip";
            
            extArgs <<  "-o" << mArchivePath;
            listArgs << "-l" << mArchivePath;
        }
    }
    else if ( mime == "application/x-7z-compressed" )
    {
        if ( which( "7z" ) != QString::null )
        {
            app = "7z";
            
            extArgs << "x" << mArchivePath;
            listArgs << "l" << mArchivePath;
        }
        else if ( which( "7zr" ) != QString::null )
        {   
            app = "7zr";
            
            extArgs <<  "x" << mArchivePath;
            listArgs << "l" << mArchivePath;
        }
    }
    else if ( mime == "application/x-ace" )
    {
        if ( which( "unace" ) != QString::null )
        {   
            app = "unace";
            
            extArgs <<  "x" << "-y" << "-c-" << mArchivePath;
            listArgs << "l" << "-y" << "-c-" << mArchivePath;
        }
    }

    const QFileInfo fileinfo( mArchivePath );
   
    if ( !fileinfo.isReadable() || extArgs.isEmpty() || listArgs.isEmpty() )
    {
        qDebug() << "[ARCHIVEEXTRACT] Exiting, fileinfo readable:" << fileinfo.isReadable() << "app:" << app << "extargs:" << extArgs << "listargs:" << listArgs;
        emit threadFinished( this );
    }
    else
    {
        extract.setWorkingDirectory( mDestination );
        
        qDebug() << "[ARCHIVEEXTRACT] Getting archive info:" << app << listArgs;
        
        info.start( app, listArgs );
        
        if ( !info.waitForFinished( -1 ) ) 
        {
            return;
        }
       
	qDebug() << "archive info finished";
 
        QString b = info.readAllStandardOutput();
        QStringList files = b.split( "\n" );
        mFilesCnt = files.size();

	qDebug() << "Files count: " << mFilesCnt;
        
        emit threadNewMaximum( mFilesCnt );
        emit threadProgress( 0, "Extracting..." );
        
        mExtFilesCnt = 0;
        QString final;
        
        qDebug() << "[ARCHIVEEXTRACT] Extracting archive:" << app << extArgs;
        
        extract.start( app, extArgs );
        
        while ( extract.waitForReadyRead() )
        {
            QString line = extract.readAllStandardOutput();
            
            if ( line.contains( "\n" ) ) 
            {
                ++mExtFilesCnt;
            }
            
            if ( ( final.isEmpty() ) && 
                ( line.contains( "/" ) ) )
            {
                final = line.trimmed().split( "/" ).at( 0 );
            }
            
            emit threadProgress( mExtFilesCnt, QString::Null() );
        }
        
        if ( !final.isEmpty() )
        {
            if ( final.contains( "x " ) )
            {
                final = final.remove( "x " );
            }
            
            emit extracted( mArchivePath, mDestination + "/" + final );
        }
        
        emit threadFinished( this );
    }
}

QString ArchiveExtract::getMimeType( const QString& file )
{
    QString result( "application/octet-stream" );
    magic_t mime = magic_open( MAGIC_MIME_TYPE );

    if ( !mime )
    {
        qDebug() << "[ARCHIVEEXTRACT] Can't init libmagic";
    }
    else
    {
        if ( magic_load( mime, 0 ) ) 
        { 
            qDebug() << "[ARCHIVEEXTRACT] Unable to load magic DB: " + QString( magic_error( mime ) );      
        }
        else
        {
            result = magic_file( mime, file.toUtf8().data() );
        }
        
        magic_close( mime );
    }

    qDebug() << "libmagic: result mime type - " + result + "for file: " + file;

    return result;
}


#include "archiveextract.moc"

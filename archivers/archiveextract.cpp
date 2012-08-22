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
#include "archivers/ArchiversConfiguration.h"
#include "archivers/Utility.h"

#include <QDebug>

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

    QStringList extArgs;
    QStringList listArgs;
    
    ArchiversConfiguration::instance().getExtractArguments( mArchivePath, extArgs, listArgs );

    const QFileInfo fileinfo( mArchivePath );
   
    if ( !fileinfo.isReadable() || extArgs.isEmpty() || listArgs.isEmpty() )
    {
        qDebug() << "[ARCHIVEEXTRACT] Exiting, fileinfo readable:" << fileinfo.isReadable() << "extargs:" << extArgs << "listargs:" << listArgs;
        emit threadFinished( this );
    }
    
    const QString extprg = extArgs.takeFirst();
    const QString infprg = listArgs.takeFirst();
    
    extract.setWorkingDirectory( mDestination );
    
    qDebug() << "[ARCHIVEEXTRACT] Getting archive info:" << infprg << listArgs;
    
    info.start( infprg, listArgs );
    
    if ( !info.waitForFinished( -1 ) ) 
    {
        return;
    }
    
    QString b = info.readAllStandardOutput();
    QStringList files = b.split( "\n" );
    mFilesCnt = files.size();
    
    emit threadNewMaximum( mFilesCnt );
    emit threadProgress( 0, "Extracting..." );
    
    mExtFilesCnt = 0;
    QString final;
    
    qDebug() << "[ARCHIVEEXTRACT] Extracting archive:" << extprg << extArgs;
    
    extract.start( extprg, extArgs );
    
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

#include "archiveextract.moc"

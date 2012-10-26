/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>

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

#include "dircleaner.h"
#include "settings.h"
#include "config.h"

#include <QDir>
#include <QFile>

DirCleaner::DirCleaner( const QStringList& dirs ) : KueueThread()
{
    qDebug() << "[DIRCLEANER] Constructing" << currentThreadId();
    mDirs = dirs;
}

DirCleaner::~DirCleaner()
{
    qDebug() << "[DIRCLEANER] Destroying";
}

void DirCleaner::run()
{         
    emit threadStarted( "Cleaning up...", mDirs.size() );
    
    for ( int i = 0; i < mDirs.size(); ++i ) 
    {
        qDebug() << "[DIRCLEANER] Deleting directory" << mDirs.at( i );
        
        QDir dir( mDirs.at( i ) );
        
    #ifndef IS_WIN32
        QProcess p;
        QStringList args;
        
        args.append( "-R" );
        args.append( "777" );
        args.append( dir.path() );
        
        p.start( "chmod", args );
        
        if (  !p.waitForFinished ( -1 ) )
        {
            return;
        }
    #endif

        if ( dir.exists() )
        {
            QDirIterator fileWalker( dir.path(), QDir::Files | QDir::Hidden | QDir::System, QDirIterator::Subdirectories );

            while( fileWalker.hasNext() )
            {
                fileWalker.next();
                QFile::remove( fileWalker.filePath() );
            }

            QDirIterator dirWalker( dir.path(), QDir::Dirs | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );

            while( dirWalker.hasNext() )
            {
                dirWalker.next();
                dir.rmpath( dirWalker.filePath() );
            }

            dir.rmdir( dir.path() );
            emit threadProgress( i, QString::Null() );
        }
    }
    
    QStringList failedList;
    
    for ( int i = 0; i < mDirs.size(); ++i ) 
    {
        QDir d( mDirs.at( i ) );
        
        if ( d.exists() )
        {
            failedList.append( mDirs.at( i ) );
        }
    }
    
    if ( failedList.size() > 0 )
    {
        emit dirsFailed( failedList );
    }
        
    emit threadFinished( this );
}

#include "dircleaner.moc"

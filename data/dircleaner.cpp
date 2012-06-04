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

#include <QDir>
#include <QFile>

DirCleaner::DirCleaner( const QStringList& dirs ) : KueueThread()
{
    mDirs = dirs;
}

DirCleaner::~DirCleaner()
{
}

void DirCleaner::run()
{         
    emit threadStarted( "Cleaning up download directories...", mDirs.size() );
    
    for ( int i = 0; i < mDirs.size(); ++i ) 
    {
        qDebug() << "[DIRCLEANER] Deleting download directory for" << mDirs.at( i );
        
        QDir dir( Settings::downloadDirectory() + "/" + mDirs.at( i ) );
        QDirIterator fileWalker( dir.path(), QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories );
    
        while( fileWalker.hasNext() )
        {
            fileWalker.next();
            QFile::remove( fileWalker.filePath() );
        }
    
        QDirIterator dirWalker( dir.path(), QDir::Dirs | QDir::NoSymLinks, QDirIterator::Subdirectories );
    
        while( dirWalker.hasNext() )
        {
            dirWalker.next();
            
            if ( dirWalker.fileName() != "." && dirWalker.fileName() != ".." )
            {
                dir.rmpath( dirWalker.filePath() );
            }
        }

        dir.rmdir( dir.path() );
        emit threadProgress( i );
    }
        
    emit threadFinished( this );
}

#include "dircleaner.moc"

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

#include "supportconfig.h"
#include "config.h"
#include "settings.h"

#include <QDebug>
#include <QObject>
#include <QProcess>

SupportConfig::SupportConfig( const QString& sc ) : KueueThread()
{
    qDebug() << "[SUPPORTCONFIG] Constructing";

    mScDir = sc;
    start();
}

SupportConfig::~SupportConfig()
{
    qDebug() << "[SUPPORTCONFIG] Destroying";
}

void SupportConfig::run()
{
    emit threadStarted( "Preparing supportconfig...", 0 );
    
    QDir dir( mScDir );
    dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
    QStringList filelist = dir.entryList();
    
    dir.mkdir( "files" );
    
    for ( int i = 0; i < filelist.size(); ++i ) 
    {
        QString fd = "files/" + filelist.at( i );
        
        dir.mkdir( fd );
        
        QFile file( mScDir + "/" + filelist.at( i ) );
        
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            return;
        }
        
        QString content = file.readAll();
        
        QStringList conffiles = content.split( "#==[ Configuration File ]===========================#\n" );
        
        for ( int x = 0; x < conffiles.size(); ++x ) 
        { 
            QFile outfile( mScDir + "/" + fd + "/file" + QString::number( x ) + ".txt" );
            
            if ( !outfile.open( QIODevice::WriteOnly ) )
            {
                return;
            }
            
            QTextStream out( &outfile );
            
            if ( conffiles.at( x ).contains( "#==[" ) )
            {
                out << conffiles.at( x ).split( "#==[" ).at( 0 );
            }
            else
            {
                out << conffiles.at( x );
            }
            
            outfile.close();
        }
    }
    
    QDirIterator fileWalker( dir.path() + "/files", QDir::Files | QDir::Hidden | QDir::System, QDirIterator::Subdirectories );

    dir.mkdir( "system" );
    
    while( fileWalker.hasNext() )
    {
        fileWalker.next();
        QFile c( fileWalker.filePath() );
        
        if ( !c.open( QIODevice::ReadOnly ) )
        {
            return;
        }
        
        QString fn = c.readLine();
        
        if ( ( !fn.isEmpty() ) && 
             ( !fn.contains( "File not found" ) ) && 
             ( fn.startsWith( "# " ) ) )
        {
            QFileInfo info( fn.remove( "# " ).trimmed() );
            QDir sysdir( dir.path() + "/system" + info.dir().path() );
            
            if ( !sysdir.exists() )
            {
                sysdir.mkpath( sysdir.path() );
            }
            
            QFile of( sysdir.path() + "/" + info.fileName() );
         
            if ( !of.open( QIODevice::WriteOnly ) )
            {
                return;
            }
            
            QTextStream out( &of );
            out << c.readAll();
            of.close();
        }
        
        QFile::remove( fileWalker.filePath() );
    }
    
    QDirIterator dirWalker( dir.path() + "/files", QDir::Dirs, QDirIterator::Subdirectories );
    
    while( dirWalker.hasNext() )
    {
        dirWalker.next();

        if ( dirWalker.fileName() != "." && dirWalker.fileName() != ".." )
        {
            dir.rmpath( dirWalker.filePath() );
        }
    }

    dir.rmdir( dir.path() + "/files" );
    
    QProcess p;
    QStringList args;
    
    args.append( "cjfv" );
    args.append( "supportconfig.tar.bz2" );
    args.append( "." );
    
    p.setWorkingDirectory( mScDir );
    p.start( "tar", args );

    if ( !p.waitForFinished ( -1 ) )
    {
        return;
    }

    emit supportconfigPrepared( mScDir );
    emit threadFinished( this );
}

#include "supportconfig.moc"
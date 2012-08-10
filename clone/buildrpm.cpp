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

#include "buildrpm.h"
#include "config.h"
#include "settings.h"

#include <QDebug>
#include <QObject>
#include <QProcess>

BuildRPM::BuildRPM( const QString& sc ) : KueueThread()
{
    qDebug() << "[BUILD] Constructing";

    mScDir = sc;
}

BuildRPM::~BuildRPM()
{
    qDebug() << "[BUILD] Destroying";
}

void BuildRPM::run()
{
    QProcess p;
    
    emit threadStarted( "Building RPM...", 0 );
    
    p.setWorkingDirectory( mScDir );
    
    p.start( "bash", QStringList() << "clone.sh" );
    
    if (  !p.waitForFinished ( -1 ) )
    {
        return;
    }
    
    QString out = p.readAllStandardOutput().trimmed(); 
    qDebug() << "[BUILDRPM] Script output:" << out;
    
    QFile res( mScDir + "/clone-result" );
    
    QStringList reslist;
    
    if ( !res.open( QFile::ReadOnly ) )
    {
        qDebug() << "[BUILDRPM] Could not open results file";
    }
    else
    {
        QTextStream stream( &res );
        
        while ( !stream.atEnd() )
        {
            QString line = stream.readLine();
            
            if ( !line.startsWith( "NAME VERS" ) )
            {
                reslist.append( line );
            }
        }
    }
    
    if ( out.startsWith( "SUCCESS" ) )
    {
        QStringList l = out.split( "/" );
        emit success( l.at( 1 ), l.at( 2 ), reslist, l.at( 3 ) );
    }
    else if ( out.startsWith( "FAILED" ) )
    {
        emit failed( out.remove( "FAILED/" ) );
    }
}

#include "buildrpm.moc"

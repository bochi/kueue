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
    qDebug() << "[BUILDRPM] Constructing";

    mScDir = sc;
    start();
}

BuildRPM::~BuildRPM()
{
    qDebug() << "[BUILDRPM] Destroying";
}

void BuildRPM::run()
{
    QProcess* p = new QProcess;
    
    emit threadStarted( "Preparing RPM...", 0 );
    
    connect( p, SIGNAL( readyRead() ), 
             this, SLOT( scriptOutput() ) );
    
    p->setWorkingDirectory( mScDir );
    
    p->start( "bash", QStringList() << "clone.sh" );
    
    if (  !p->waitForFinished ( -1 ) )
    {
        return;
    }
}

void BuildRPM::scriptOutput()
{
    QProcess* p = qobject_cast< QProcess* >( sender() );
    QString out = p->readAllStandardOutput();
    QStringList lines;

    if( out.contains( "\n" ) )
    {
        lines = out.split( "\n" );
    }
    else
    {
        lines.append( out );
    }
    
    for ( int i = 0; i < lines.size(); ++i )
    {
        QString o = lines.at( i );
        
        if ( o.startsWith( "TOTAL" ) )
        {
            int t = o.remove( "TOTAL " ).toInt();
            emit threadStarted( "Building RPM...", t );
        }
        else if ( o.startsWith( "PROG" ) )
        {
            int p = o.remove( "PROG " ).toInt();
            emit threadProgress( p );
        }
        else if ( o.startsWith( "SUCCESS" ) )
        {
            QStringList l = o.split( "/" );
            scriptSuccess( l.at( 1 ), l.at( 2 ), l.at( 3 ) );
        }
        else if ( o.startsWith( "FAILED" ) )
        {
            emit failed( o.remove( "FAILED/" ) );
            emit threadFinished( this );
        }
    }
}

void BuildRPM::scriptSuccess( const QString& prod, const QString& arch, const QString& host )
{
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

    emit success( prod, arch, reslist, host );
    emit threadFinished( this );
}

#include "buildrpm.moc"

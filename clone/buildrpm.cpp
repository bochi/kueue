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

#include "build.h"
#include "config.h"
#include "settings.h"

#include <QDebug>
#include <QObject>
#include <QProcess>

Build::Build( const QString& sc ) : KueueThread()
{
    qDebug() << "[BUILD] Constructing";

    mScDir = sc;
}

Build::~Build()
{
    qDebug() << "[BUILD] Destroying";
}

void Build::run()
{
    QProcess p;
    
    emit threadStarted( "Building RPM...", 0 );
    
    p.setWorkingDirectory( mScDir );
    
    p.start( "bash", QStringList() << "clone.sh" );
    
    if (  !p.waitForFinished ( -1 ) )
    {
        return;
    }
    
    QString out = p.readAllStandardOutput(); 
    
    qDebug() << out;
}

#include "build.moc"

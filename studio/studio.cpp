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

#include "studio.h"
#include "studiojob.h"
#include "kueue.h"
#include "kueuethreads.h"
#include "archivers/archiveextract.h"

#include <QFileDialog>
#include <QToolBar>
#include <QDesktopServices>
#include <QDebug>
#include <QtXml>

Studio::Studio( const QString& sc )
{
    qDebug() << "[STUDIO] Constructing";
    
    mSupportConfig = sc;
    QDir tmpdir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/kueue-studio";
    
    if ( !tmpdir.exists() )
    {
        tmpdir.mkpath( tmpdir.absolutePath() );
    }
    
    ArchiveExtract* x = new ArchiveExtract( mSupportConfig, mTmpDir.absolutePath() );
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( createThread( QString, QString ) ) );
    
    KueueThreads::enqueue( x );
}

Studio::~Studio()
{
    qDebug() << "[STUDIO] Destroying";
}

void Studio::createThread( const QString& scfile, const QString& scdir )
{
    StudioJob* j = new StudioJob( scdir );
    
    connect( j, SIGNAL( threadFinished( KueueThread* ) ),
             this, SLOT( studioThreadFinished() ) );
    
    KueueThreads::enqueue( j );
}

void Studio::studioThreadFinished()
{
    // do something?
    emit buildFinished();
}

#include "studio.moc"

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

#include "clone.h"
#include "studio.h"
#include "build.h"
#include "kueue.h"
#include "kueuethreads.h"
#include "archivers/archiveextract.h"

#include <QFileDialog>
#include <QToolBar>
#include <QDesktopServices>
#include <QDebug>
#include <QtXml>

Clone::Clone( const QString& sc )
{
    qDebug() << "[STUDIO] Constructing";
    
    mSupportConfig = sc;
    QDir tmpdir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/kueue-studio";
    
    if ( !tmpdir.exists() )
    {
        tmpdir.mkpath( tmpdir.absolutePath() );
    }
    
    ArchiveExtract* x = new ArchiveExtract( mSupportConfig, tmpdir.absolutePath() );
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( downloadScript( QString, QString ) ) );
    
    KueueThreads::enqueue( x );
}

Clone::~Clone()
{
    qDebug() << "[STUDIO] Destroying";
}

void Clone::downloadScript( const QString& archive, const QString& dir )
{
    mScDir = dir;
    QEventLoop loop;
    QNetworkReply* r;

    r = Network::getExt( QUrl( "http://w3.suse.de/~ajohansson/clone.sh" ) );
        
    connect( r, SIGNAL( finished() ), 
             this, SLOT( scriptDownloadDone() ) );
}

void Clone::scriptDownloadDone()
{
    QNetworkReply* r = qobject_cast< QNetworkReply* >( sender() );
                    
    QFile file( mScDir.absolutePath() + "/clone.sh" );
    
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        return;
    }
    
    file.write( r->readAll() );
    file.close();
   
    Build* build = new Build( mScDir.absolutePath() );
    
    connect( build, SIGNAL( threadFinished( KueueThread* ) ),
             this, SLOT( buildAppliance() ) );
    
    KueueThreads::enqueue( build );
    buildAppliance();
}

void Clone::buildAppliance()
{
    Studio* studio = new Studio( mScDir.absolutePath() );
    
    connect( studio, SIGNAL( threadFinished( KueueThread* ) ),
             this, SLOT( cloneDone() ) );
}

void Clone::cloneDone()
{
}

#include "clone.moc"

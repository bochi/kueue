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

#include "nsa.h"
#include "nsajob.h"
#include "kueue.h"
#include "kueuethreads.h"
#include "archivers/archiveextract.h"
#include "ui/html.h"

#include <QFileDialog>
#include <QToolBar>
#include <QDesktopServices>
#include <QDebug>
#include <QtXml>

NSA::NSA( const QString& sc, const QString& save )
{
    qDebug() << "[NSA] Constructing";
    
    mSupportConfig = sc;
    mSaveLocation = save;
    
    if ( mSupportConfig.endsWith( ".tbz" ) || mSupportConfig.endsWith( ".bz2" ) )
    {
        mIsArchive = true;
    }
    else
    {
        mIsArchive = false;
    }
    
    mNsaDir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/nsa" );
    
    if ( !mNsaDir.exists() )
    {
        mNsaDir.mkpath( mNsaDir.absolutePath() );
        Settings::setNsaVersion( "new" );
    }
    
    mTmpDir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/kueue/nsa";
    
    if ( !mTmpDir.exists() )
    {
        mTmpDir.mkpath( mTmpDir.absolutePath() );
    }
    
    updateNsaData();
}

NSA::~NSA()
{
    qDebug() << "[NSA] Destroying";
}

void NSA::updateNsaData()
{
    QNetworkReply* r = Network::getExt( QUrl( "http://" + Settings::dBServer() + "/nsa/current.txt" ) );
        
    connect( r, SIGNAL( finished() ), 
             this, SLOT( updateDownloadDone() ) );
}

void NSA::updateDownloadDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( QObject::sender() );
    QString date = reply->readAll();
    
    if ( Settings::nsaVersion() != date.trimmed() )
    {
        qDebug() << "[NSA] New patterns available, downloading.";
        
        QNetworkReply* r = Network::getExt( QUrl( "http://" + Settings::dBServer() + "/nsa/current.tgz" ) );
        
        Settings::setNsaVersion( date.trimmed() );
        
        connect( r, SIGNAL( finished() ), 
                 this, SLOT( fileDownloadDone() ) );   
    }
    else
    {
        startReport();
    }
}

void NSA::fileDownloadDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( QObject::sender() );

    QFile file( mNsaDir.absolutePath() + "/current.tgz" );
    
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        return;
    }
    
    file.write( reply->readAll() );
    file.close();
    
    ArchiveExtract* x = new ArchiveExtract( file.fileName(), mNsaDir.absolutePath() );
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( startReport() ) );
    
    KueueThreads::enqueue( x );
}

void NSA::startReport()
{
    if ( mIsArchive )
    {
        ArchiveExtract* x = new ArchiveExtract( mSupportConfig, mTmpDir.absolutePath() );
        
        connect( x, SIGNAL( extracted( QString, QString ) ),
                this, SLOT( runPS( QString, QString ) ) );
        
        KueueThreads::enqueue( x );
    }
    else
    {
        runPS( mSupportConfig, mSupportConfig );
    }
}

void NSA::runPS( const QString& scfile, const QString& scdir )
{
    NSAJob* j = new NSAJob( scfile, scdir );
    
    connect( j, SIGNAL( finishedReport( const QString& ) ),
             this, SLOT( ranPS( const QString& ) ) );
    
    KueueThreads::enqueue( j );
}

void NSA::ranPS( const QString& html )
{
    emit nsaReportFinished();
    
    if ( mSaveLocation.isNull() )
    {
        NSAWindow* win = new NSAWindow( QFileInfo( mSupportConfig ).fileName(), html );
        
        connect( win, SIGNAL( done( NSAWindow* ) ),
                this, SLOT( winClosed( NSAWindow* ) ) );
        
        win->show();
    }
    else
    {
        QFile file( mSaveLocation );
    
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            return;
        }
    
        file.write( html.toUtf8() );
        file.close();
    }
}

void NSA::winClosed( NSAWindow* win )
{
    delete win;
    delete this;
}

#include "nsa.moc"

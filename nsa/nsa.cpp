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
#include "unity/archiveextract.h"
#include "ui/html.h"

#include <QFileDialog>
#include <QToolBar>
#include <QDesktopServices>
#include <QDebug>
#include <QtXml>

NSA::NSA( const QString& sc )
{
    qDebug() << "[NSA] Constructing";
    
    mSupportConfig = sc;
    
    mNsaDir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/nsa" );
    mTmpDir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/nsa";
    
    if ( !mNsaDir.exists() )
    {
        mNsaDir.mkpath( mNsaDir.absolutePath() );
        Settings::setNsaVersion( "new" );
    }
    
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
    QNetworkReply* r = Network::getExt( QUrl( "http://nsa.lab.novell.com/updateDir/updateManager.xml" ) );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( updateDownloadDone() ) );
}

void NSA::updateDownloadDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( QObject::sender() );
    QString out = reply->readAll();
    QDomDocument xmldoc;
    xmldoc.setContent(out);
    QDomNodeList list = xmldoc.elementsByTagName( "sdppackage" );
    QString date = list.item(0).toElement().text();

    if ( Settings::nsaVersion() != date )
    {
        QNetworkReply* r = Network::getExt( QUrl( "http://nsa.lab.novell.com/updateDir/resources/resources_" + date + ".tgz" ) );
        Settings::setNsaVersion( date );
        
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
    
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Text ) )
    {
        return;
    }
    
    file.write( reply->readAll() );
    
    #ifndef IS_WIN32  
    ArchiveExtract* x = new ArchiveExtract( file.fileName(), mNsaDir.absolutePath() );
    KueueThreads::enqueue( x );  
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( startReport() ) );
    #endif
}

void NSA::startReport()
{
    #ifndef IS_WIN32
    ArchiveExtract* x = new ArchiveExtract( mSupportConfig, mTmpDir.absolutePath() );
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( runPS( QString, QString ) ) );
    
    KueueThreads::enqueue( x ); 
    #endif
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
    NSAWindow* win = new NSAWindow( QFileInfo( mSupportConfig ).fileName(), html );
    
    connect( win, SIGNAL( done( NSAWindow* ) ),
             this, SLOT( winClosed( NSAWindow* ) ) );
    
    win->show();
}

void NSA::winClosed( NSAWindow* win )
{
    delete win;
    delete this;
}

#include "nsa.moc"

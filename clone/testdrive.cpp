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

#include "testdrive.h"
#include "config.h"
#include "settings.h"
#include "network.h"
#include "qstudio.h"
#include "kueuethreads.h"
#include "archivers/archiveextract.h"

#include <QDebug>
#include <QObject>

TestDrive::TestDrive( int build ) : QObject()
{
    qDebug() << "[TESTDRIVE] Constructing";

    mVncWidget = new VncWidget( RemoteView::TestDrive );
    mThread = new TestDriveThread( build );
    
    connect( mThread, SIGNAL( vnc( QUrl ) ),
             mVncWidget, SLOT( createVncView( QUrl ) ) );
    
    //connect( mVncWidget, SIGNAL( somethingWentWrong() ), 
    //         mThread, SLOT( requestNewTestdrive() ) );
        
    connect( mVncWidget, SIGNAL( downloadRequested() ), 
             mThread, SLOT( downloadAppliance() ) );
    
    connect( mThread, SIGNAL( downloadRequested( QString ) ), 
             this, SLOT( addDownload( QString ) ) );
}

TestDrive::~TestDrive()
{
    qDebug() << "[TESTDRIVE] Destroying";
    
    mThread->deleteWorker();
    mThread->quit();
    mThread->wait();
    
    delete mVncWidget;
    delete mThread;
}

void TestDrive::setTabId( int id )
{
    mTabId = id;
}

void TestDrive::addDownload( const QString& url )
{
    mThread->setDoNotQuit( true );
    
    QNetworkReply* reply = Network::getExt( url );
    
    connect( reply, SIGNAL( finished() ),
             this, SLOT( downloadFinished() ) );
    
    StatusBar::addDownloadJob( reply, Settings::applianceDownloadDirectory(), false, false );
}

void TestDrive::downloadFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    QUrl u = reply->url();
    
    QString filename = reply->objectName();
    
    ArchiveExtract* x = new ArchiveExtract( filename, QFileInfo( filename ).dir().path() );

    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( extractFinished( QString, QString ) ) );

    KueueThreads::enqueue( x );
}

void TestDrive::extractFinished( const QString& filename, const QString& dir )
{
    QFile::remove( filename );
    QDir directory( dir );
    directory.setNameFilters( QStringList() << "*.vmdk" );
    directory.setFilter( QDir::Files );
    directory.setSorting( QDir::Name );
    
    QString vmdk = directory.entryList().first();
    
    mThread->setDoNotQuit( false );
}

// TestDriveThread

TestDriveThread::TestDriveThread( int build ) : QThread()
{
    mBuildId = build;
    start();
}

TestDriveThread::~TestDriveThread()
{
}

void TestDriveThread::deleteWorker()
{
    emit deleteWorkerSignal();
}

void TestDriveThread::setDoNotQuit( bool q )
{
    emit doNotQuit( q );
}

void TestDriveThread::run()
{
    mWorker = new TestDriveWorker( mBuildId );
    
    connect( mWorker, SIGNAL( vnc( QUrl ) ), 
             this, SIGNAL( vnc( QUrl ) ) );
    
    connect( this, SIGNAL( newTestdriveRequested() ), 
             mWorker, SLOT( newTestdriveRequested() ) );
    
    connect( mWorker, SIGNAL( timedOut( int ) ), 
             this, SIGNAL( timedOut( int ) ) );
    
    connect( mWorker, SIGNAL( downloadRequested( QString ) ),
             this, SIGNAL( downloadRequested( QString ) ) );
    
    connect( this, SIGNAL( downloadApplianceRequested() ), 
             mWorker, SLOT( downloadAppliance() ) );
    
    connect( this, SIGNAL( doNotQuit( bool ) ),
             mWorker, SLOT( setDoNotQuit( bool ) ) );
    
    connect( this, SIGNAL( deleteWorkerSignal() ), 
             mWorker, SLOT( killMe() ) );
    
    mWorker->work();
    
    exec();
}

void TestDriveThread::requestNewTestdrive()
{
    emit newTestdriveRequested();
}

void TestDriveThread::downloadAppliance()
{
    emit downloadApplianceRequested();
}

// TESTDRIVE WORKER

TestDriveWorker::TestDriveWorker( int build ) : QObject()
{
    qDebug() << "[TESTDRIVEWORKER] Constructing" << this->thread()->currentThreadId();
    
    mDoNotQuit = false;
    mBuildId = build;
    mStudio = new QStudio( Settings::studioServer(), Settings::studioUser(), Settings::studioApiKey(), Settings::studioDebugEnabled() );
    mTimer = new QTimer( this );
    
    connect( mTimer, SIGNAL( timeout() ), 
             this, SLOT( checkTestdrive() ) );
}

TestDriveWorker::~TestDriveWorker()
{
    while ( mDoNotQuit ) 
    {
        qDebug() << "Waiting..";
    }
    
    delete mStudio;
    qDebug() << "[TESTDRIVEWORKER] Destroying";
}

void TestDriveWorker::work()
{
    getTestdriveForBuild();
    mTimer->start( 10000 );
}

void TestDriveWorker::killMe()
{
    delete this;
}

void TestDriveWorker::setDoNotQuit( bool q )
{
    mDoNotQuit = q;
}

void TestDriveWorker::getTestdriveForBuild()
{
    Testdrive td = mStudio->getTestdrive( mBuildId );
    mTestDriveId = td.id;
    mUrl = "vnc://:" + td.vncpassword + "@" + td.vnchost + ":" + td.vncport;
    emit vnc( mUrl );
}

void TestDriveWorker::checkTestdrive()
{
    QList<UserTestDrive> tdl = mStudio->getUserTestdrives();
    bool ok = false;
    
    for ( int i = 0; i < tdl.size(); ++i )
    {
        if ( tdl.at( i ).testdriveid == mTestDriveId )
        {
            ok = true;
        }
    }
    
    if ( !ok )
    {
        qDebug() << "[TESTDRIVEWORKER] Timed out.";
        mTimer->stop();
        newTestdriveRequested();
    }
}

void TestDriveWorker::newTestdriveRequested()
{
    qDebug() << "[TESTDRIVEWORKER] New Testdrive requested.";
    
    QList<UserTestDrive> tdl = mStudio->getUserTestdrives();
    bool ok = false;
    
    for ( int i = 0; i < tdl.size(); ++i )
    {
        if ( tdl.at(i).testdriveid == mTestDriveId )
        {
            ok = true;
        }
    }
    
    if ( !ok )
    {
        qDebug() << "[TESTDRIVEWORKER] Not OK - starting over.";
        mTimer->stop();
        work();
    }
    else
    {
        qDebug() << "[TESTDRIVEWORKER] Testdrive for this build exists.";
        emit vnc( mUrl );
    }
}

void TestDriveWorker::downloadAppliance()
{
    QString url = mStudio->getDownloadUrlForBuild( mBuildId );

    emit downloadRequested( url );
}

#include "testdrive.moc"

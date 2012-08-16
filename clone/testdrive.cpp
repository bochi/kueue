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
    
    mDoNotQuit = false;
    mVncWidget = new VncWidget( this, RemoteView::TestDrive );
    mThread = new TestDriveThread( build );
    
    connect( mThread, SIGNAL( vnc( QUrl ) ),
             mVncWidget, SLOT( createVncView( QUrl ) ) );
    
    //connect( mVncWidget, SIGNAL( somethingWentWrong() ), 
    //         mThread, SLOT( requestNewTestdrive() ) );
    
    connect( mVncWidget, SIGNAL( widgetClosed( int ) ), 
             this, SLOT( quitTestdrive( int ) ) );
    
    connect( mVncWidget, SIGNAL( downloadRequested() ), 
             mThread, SLOT( downloadAppliance() ) );
    
    connect( mThread, SIGNAL( downloadRequested(QString)), 
             this, SLOT( addDownload(QString)) );
}

TestDrive::~TestDrive()
{
    qDebug() << "[TESTDRIVE] Destroying";
}

void TestDrive::addDownload( const QString& url )
{
    mDoNotQuit = true;
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
    const QStringList& list = directory.entryList( QDir::Files );
    QString vmx;
    
    for ( int i = 0; i < list.size(); ++i )
    {
        if ( list.at( i ).endsWith( ".vmx" ) )
        {
            vmx = list.at( i );
        }
    }
    
    QProcess::startDetached( "vmware", list, directory.path() );
    
    mDoNotQuit = false;
}

void TestDrive::quitTestdrive( int id )
{
    while ( mDoNotQuit )
    {
        qDebug() << "Can't quit yet - waiting";
    }
    
    mThread->deleteWorker();
    mThread->quit();
    mThread->wait();

    delete mThread;
    delete mVncWidget;
    
    emit testdriveClosed( id );
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
    delete mWorker;
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
    
    mBuildId = build;
    mStudio = new QStudio( Settings::studioServer(), Settings::studioUser(), Settings::studioApiKey(), Settings::studioDebugEnabled() );
    mTimer = new QTimer( this );
    
    connect( mTimer, SIGNAL( timeout() ), 
             this, SLOT( checkTestdrive() ) );
}

TestDriveWorker::~TestDriveWorker()
{
    delete mStudio;
    qDebug() << "[TESTDRIVEWORKER] Destroying";
}

void TestDriveWorker::work()
{
    getTestdriveForBuild();
    mTimer->start( 10000 );
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
    //QNetworkAccessManager* nam = new QNetworkAccessManager( this );
    
    QString url = mStudio->getDownloadUrlForBuild( mBuildId );
    //const QUrl& u( url );
    
    //QNetworkRequest request( u );
    //QString filename = QFileInfo( u.path() ).completeBaseName() + "." + QFileInfo( u.path() ).suffix();
    
    //QNetworkReply* reply = nam->get( request );
    emit downloadRequested( url );
    
    //connect( reply, SIGNAL( finished() ), 
      //       this, SLOT( downloadDone() ) );
    
    /*connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ),
             this, SLOT( downloadProgress( qint64, qint64 ) ) );*/
    
    if ( url.isEmpty() )
    {
        qDebug() << "No URL yet, please try again.";
    }
    else
    {
        qDebug() << url;
    }
}

void TestDriveWorker::downloadProgress( qint64 r, qint64 t )
{
    int progress = 0;
    
    if ( t != 0 )
    {
        progress = r * 100 / t;
    }
    qDebug() << "progress" << r << t << progress;
}


void TestDriveWorker::downloadDone()
{
    QNetworkReply* reply = qobject_cast< QNetworkReply* >( sender() );
    

    
    //QFile file( Settings::applianceDownloadDirectory() + "/" + filename );
    //qDebug() << "downloading to" << file.fileName() ;
    
    /*if ( !file.open( QIODevice::WriteOnly ) )
    {
        return;
    }
    
    file.write( reply->readAll() );
    file.close();*/
}


#include "testdrive.moc"

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

TestDrive::TestDrive( int build, const QString& hostname ) : QObject()
{
    qDebug() << "[TESTDRIVE] Constructing";

    mVncWidget = new VncWidget( "Testdrive - " + hostname, RemoteView::TestDrive, true );
    mThread = new TestDriveThread( build );
    
    connect( mThread, SIGNAL( vnc( QUrl ) ),
             mVncWidget, SLOT( createVncView( QUrl ) ) );
    
    //connect( mVncWidget, SIGNAL( somethingWentWrong() ), 
    //         mThread, SLOT( requestNewTestdrive() ) );
        
    connect( mVncWidget, SIGNAL( downloadRequested() ), 
             mThread, SLOT( downloadAppliance() ) );
    
    connect( mVncWidget, SIGNAL( closeRequested() ),
             this, SLOT( closeTestdrive() ) );
    
    connect( mThread, SIGNAL( downloadRequested( QString ) ), 
             this, SIGNAL( downloadRequested( QString ) ) );
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

void TestDrive::closeTestdrive()
{
    emit closeRequested( mTabId );
}

void TestDrive::setTabId( int id )
{
    mTabId = id;
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
    QString url = mStudio->getDownloadUrlForBuild( mBuildId );

    emit downloadRequested( url );
}

#include "testdrive.moc"

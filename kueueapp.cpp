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

#include "kueueapp.h"
#include "ui/html.h"
#include "config.h"
#include "network.h"
#include "settings.h"
#include "kueue.h"
#include "ui/configdialog.h"
#include "ui/updatedialog.h"
#include "nsa/nsa.h"

#include "ui/busywidget.h"

#include <QProgressDialog>
#include <QShortcut>
#include <QKeySequence>
#include <QDesktopServices>

KueueApp::KueueApp()
{
    qDebug() << "[KUEUE] Constructing";

    if ( !Settings::settingsOK() )
    {
        BasicConfig* bc = new BasicConfig();
        int reply = bc->exec();
        delete bc;
        
        if ( reply == 0 )
        {
            // this will segfault, but we are quitting anyway :P
            delete this;
        }
        else
        {
            createApp();
        }
    }
    else
    {
        createApp();
    }
}

KueueApp::~KueueApp()
{
    qDebug() << "[KUEUE] Destroying";
    cleanupTemp();
    if ( mWindow != 0 ) mWindow->destroy();
}

void KueueApp::cleanupTemp()
{
    QDir dir( QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/nsa" );
    QDirIterator dirWalker( dir.path(), QDir::Files | QDir::Dirs | QDir::NoSymLinks, QDirIterator::Subdirectories );
    
    while( dirWalker.hasNext() )
    {
        dirWalker.next();
        QFileInfo info( dirWalker.filePath() );
        
        if ( info.isDir() )
        {
            QDir d( dirWalker.filePath() );
            dir.rmpath( dirWalker.filePath() );
        }
        else
        {
            QFile::remove( dirWalker.filePath() );
        }
    }
}

void KueueApp::createApp()
{
    createSystray();
    createDatabase();
    createMainWindow();
    createQueue();
    createQmon();
    createStats();
   
    if ( Settings::appVersion() != QApplication::applicationVersion() )
    {
        UpdateDialog* ud = new UpdateDialog( this );
        
        if ( ud->smthToSay() )
        {
            ud->exec();
        }
        
        delete ud;
        
        qDebug() << "[KUEUE] New version, rebuilding DB" << Settings::appVersion() << "vs" << QApplication::applicationVersion();
        Settings::setAppVersion( QApplication::applicationVersion() );
        mDB->newDB( false );
    }   

    QShortcut* testNotification = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ), mWindow );
    QShortcut* newUnityTab = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_U ), mWindow );
    QShortcut* dbrebuild = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_R ), mWindow );
   
    QNetworkReply* r = Kueue::download( QUrl( "http://kueue.hwlab.suse.de:8080/latestkueue" ) );
        
    connect( r, SIGNAL( finished() ),
             this, SLOT( updateJobDone() ) );    
    connect( dbrebuild, SIGNAL( activated() ), 
             mDB, SLOT( newDB() ) ); 
    connect( testNotification, SIGNAL( activated() ),
             this, SLOT( sendTestNotification() ) );
    connect( newUnityTab, SIGNAL( activated() ),
             mTabWidget, SLOT( addUnityBrowser() ) );
}

void KueueApp::updateJobDone()
{
    QNetworkReply* r = qobject_cast< QNetworkReply* >( sender() );
    QString u = r->readAll();
    
    if ( ( u != QApplication::applicationVersion() ) && ( QApplication::applicationVersion() != "git" ) )
    {
        Kueue::notify( "kueue-general", "Update available", "<b>New kueue version available!</b><br>Please update kueue.", "" );
    }
}

void KueueApp::createSystray()
{
    mSystray = &mSystray->tray();
    
    connect( mSystray, SIGNAL( destroyed() ),
             this, SLOT( createSystray() ) );
}

void KueueApp::createQmon()
{
    mQmon = new Qmon();

    connect( mQmon, SIGNAL( qmonDataChanged() ), 
             mTabWidget, SLOT( updateQmonBrowser() ) );
    connect( mQmon, SIGNAL( initialUpdate( int,int ) ), 
             this, SLOT( updateProgress( int,int ) ) );
    connect( mQmon, SIGNAL( destroyed() ), 
             this, SLOT( createQmon() ) );
}

void KueueApp::createQueue()
{
    mQueue = new Queue();

    connect( mQueue, SIGNAL( queueDataChanged() ), 
             mTabWidget, SLOT( updateQueueBrowser() ) );
    connect( mQueue, SIGNAL( destroyed() ), 
             this, SLOT( createQueue() ) );
    connect( mQueue, SIGNAL( initialUpdate( int, int ) ),
             this, SLOT( updateProgress( int, int ) ) );
}

void KueueApp::createDatabase()
{
    mDB = new Database();
    
    connect( mDB, SIGNAL( destroyed() ), 
             this, SLOT( createDatabase() ) );
    connect( mDB, SIGNAL( dbDeleted() ), 
             this, SLOT( settingsChanged() ) );   
}

void KueueApp::createStats()
{
    mStats = new Stats();
       
    connect( mStats, SIGNAL( statsChanged() ), 
             mTabWidget, SLOT( updateStatsBrowser() ) );
    connect( mStats, SIGNAL( destroyed() ), 
             this, SLOT( createStats() ) );
    connect( mStats, SIGNAL( initialUpdate( int, int ) ), 
             this, SLOT( updateProgress( int, int ) ) );
}

void KueueApp::createMainWindow()
{
    if ( !mDB )
    {
        qDebug() << "[KUEUE] No database, creating the DB first.";
        createDatabase();
    }
    
    mWindow = &mWindow->win();
    
    mWindow->setWindowIcon( QIcon(":/icons/kueue.png").pixmap( QSize( 22, 22 ) ) );
     
    mTabWidget = &mTabWidget->tw();
    mStatusBar = &mStatusBar->getInstance();
    
    mTabWidget->setDocumentMode( true );
    
    mWindow->setCentralWidget( mTabWidget );
    mWindow->setStatusBar( mStatusBar );
    
    connect( mTabWidget, SIGNAL( openConfig() ), 
             this, SLOT( openConfig() ) );
        
    if ( Settings::showAppWindow() )
    {
        mWindow->showWindow();
    }
}

void KueueApp::updateUiData()
{
    mTabWidget->updateQueueBrowser();
    mTabWidget->updateQmonBrowser();
    mTabWidget->updateStatsBrowser();   
}

void KueueApp::openConfig()
{
    ConfigDialog* c = new ConfigDialog( mWindow );
    
    connect( c, SIGNAL( settingsChanged() ), 
             this, SLOT( settingsChanged() ) );
    
    c->exec();

    delete c;
}

void KueueApp::settingsChanged()
{
    #ifndef IS_WIN32
    #ifndef IS_OSX
    
        mSystray->destroy();
        createSystray();
    
    #endif
    #endif

    delete mDB;
    delete mQmon;
    delete mQueue;
    delete mStats;
    
    mTabWidget->refreshTabs();
    setTabPosition();
    updateUiData();
}

void KueueApp::setTabPosition()
{
    mTabWidget->setTabsPosition();
}

void KueueApp::updateProgress( int max, int type )
{
    QProgressDialog* pd = new QProgressDialog( mWindow );
    
    if ( type == 1 ) 
    {
        pd->setWindowTitle( "Queue Monitor" );
        pd->setLabelText( "Downloading queue monitor data..." );
        
        connect( mQmon, SIGNAL( initialUpdateProgress( int ) ), 
                 pd, SLOT( setValue( int ) ) );
        connect( mQmon, SIGNAL( initialUpdateDone() ), 
                 pd, SLOT( close() ) );
    }
    else if ( type == 2 )
    {
        pd->setWindowTitle( "Personal Queue" );
        pd->setLabelText( "Downloading personal queue data..." );
       
        connect( mQueue, SIGNAL( initialUpdateProgress( int ) ), 
                 pd, SLOT( setValue( int ) ) );
        connect( mQueue, SIGNAL( initialUpdateDone() ), 
                 pd, SLOT( close() ) );
    }
    else if ( type == 3 )
    {
        pd->setWindowTitle( "Stats" );
        pd->setLabelText( "Downloading stats data..." );
        
        connect( mStats, SIGNAL( initialUpdateProgress( int ) ), 
                 pd, SLOT( setValue( int ) ) );
        connect( mStats, SIGNAL( initialUpdateDone() ), 
                 pd, SLOT( close() ) );
    }    
    
    pd->setCancelButton( 0 );
    pd->setMaximum( max );
    pd->setWindowModality( Qt::WindowModal );
    pd->show();
}

void KueueApp::sendTestNotification()
{
    Kueue::notify( "kueue-monitor-low", "Test Notification", "This is a <b>test notification</b> of type<br><br>kueue-monitor-low", "12345678910" );
}

#include "kueueapp.moc"

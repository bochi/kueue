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
#include "data/dircleaner.h"
#include "data/datathread.h"

#include "ui/busywidget.h"

#include <QProgressDialog>
#include <QShortcut>
#include <QKeySequence>
#include <QDesktopServices>
#include <QCoreApplication>
#include <stdlib.h>

KueueApp::KueueApp()
{
    qDebug() << "[KUEUE] Constructing";

#ifdef IS_WIN32
    char const *const path_env = getenv( "PATH" );
    QString path = "PATH=" + QCoreApplication::applicationDirPath() + "/perl/perl/bin/;" + QCoreApplication::applicationDirPath() + "/archive/;" + path_env;

    putenv( path.toAscii() );
#endif
    
    if ( !Settings::settingsOK() )
    {
        BasicConfig* bc = new BasicConfig();
        int reply = bc->exec();
        delete bc;
        
        if ( reply == 0 )
        {
            qApp->quit();
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

void KueueApp::deleteDirs( QStringList dirs )
{
    int reply = QMessageBox::Yes;
    
    if ( dirs.size() > 5 )
    {
        QMessageBox* box = new QMessageBox;
        box->setText( "I am about to delete " + QString::number( dirs.size() ) + " directories from your download directory. Continue?" );
        box->setWindowTitle( "Really delete?" );
        box->setStandardButtons(  QMessageBox::Yes | QMessageBox::No );
        box->setDefaultButton( QMessageBox::No );
        box->setIcon( QMessageBox::Critical );
        
        reply = box->exec();
    }
    
    if ( reply == QMessageBox::Yes )
    {
        DirCleaner* d = new DirCleaner( dirs );
        
        connect( d, SIGNAL( dirsFailed( QStringList ) ), 
                 this, SLOT( deleteDirsFailed( QStringList ) ) );
        
        KueueThreads::enqueue( d );
    }
}

void KueueApp::deleteDirsFailed( QStringList fail )
{
    QString text;
    
    text += "Failed to delete the following directories:<br><br>";
    
    for ( int i = 0; i < fail.size(); ++i ) 
    {
        text += fail.at( i ) + "<br>";
    }
    
    text += "<br>Please delete these manually.<br>";
    
    QMessageBox* box = new QMessageBox;
    box->setText( text );
    box->setWindowTitle( "Delete failed" );
    box->setStandardButtons(  QMessageBox::Ok );
    box->setDefaultButton( QMessageBox::Ok );
    box->setIcon( QMessageBox::Critical );
    
    box->exec();
}

void KueueApp::createApp()
{
    bool update = false;
    
    if ( Settings::appVersion() != QApplication::applicationVersion() )
    {
        update = true;
        QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
        qDebug() << "Removing" << dir.path() + "/database.sqlite";
        QFile::remove( dir.path() + "/database.sqlite" );
    }
       
    createSystray();
    createDatabase();
    createMainWindow();
        
    mDataThread = &mDataThread->thread();
    connectDataThread();

    if ( update )
    {
        /*UpdateDialog* ud = new UpdateDialog( this );
        
        qDebug() << "[KUEUE] Update detected: " << Settings::appVersion() << "vs." << QApplication::applicationVersion();
        qDebug() << "[KUEUE] Updatedialog? Anything you'd like to share?";
        
        if ( ud->smthToSay() )
        {
            ud->exec();
        }
        
        qDebug() << "[KUEUE] OK, thanks. I'll delete you and rebuild the DB.";
        
        delete ud;*/
        
        Settings::setAppVersion( QApplication::applicationVersion() );
    }
    
    QShortcut* testNotification = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ), mWindow );
    QShortcut* newUnityTab = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_U ), mWindow );
    QShortcut* dbrebuild = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_R ), mWindow );
   
    QNetworkReply* r = Network::get( "latestkueue" );
        
    connect( r, SIGNAL( finished() ),
             this, SLOT( updateJobDone() ) );    
    connect( testNotification, SIGNAL( activated() ),
             this, SLOT( sendTestNotification() ) );
    connect( newUnityTab, SIGNAL( activated() ),
             mTabWidget, SLOT( addUnityBrowser() ) );
    connect( dbrebuild, SIGNAL( activated() ),
             this, SLOT( newDB() ) );
}

void KueueApp::newDB()
{
    Database::newDB( true );
}

void KueueApp::connectDataThread()
{
    qDebug() << "[KUEUEAPP] My thread ID is" << thread()->currentThreadId();
    
    connect( mDataThread, SIGNAL( queueDataChanged( QString ) ), 
             mTabWidget, SLOT( updateQueueBrowser( const QString& ) ) );

    connect( mDataThread, SIGNAL( qmonDataChanged( QString ) ), 
             mTabWidget, SLOT( updateQmonBrowser( const QString& ) ) );
    
    connect( mDataThread, SIGNAL( statsDataChanged( QString ) ),
             mTabWidget, SLOT( updateStatsBrowser(QString) ) );

    connect( mDataThread, SIGNAL( netError() ), 
             this, SLOT( openConfig() ) );
    
    connect( mDataThread, SIGNAL( dirsToDelete( QStringList ) ),
             this, SLOT( deleteDirs( QStringList ) ) );
}

void KueueApp::updateJobDone()
{
    QNetworkReply* r = qobject_cast< QNetworkReply* >( sender() );
    QString u = r->readAll();
    
    if ( ( u != QApplication::applicationVersion() ) && 
         ( QApplication::applicationVersion() != "git" ) )
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

void KueueApp::createDatabase()
{
    Database::openDbConnection( "sqliteDB" );
}

void KueueApp::createMainWindow()
{
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

    mDataThread->destroy();
    mDataThread = &mDataThread->restart();
    connectDataThread();
    
    mTabWidget->refreshTabs();
    setTabPosition();
    updateUiData();
}

void KueueApp::setTabPosition()
{
    mTabWidget->setTabsPosition();
}

void KueueApp::sendTestNotification()
{
    Kueue::notify( "kueue-sr-update", "Test Notification", "This is a <b>test notification</b> of type<br><br>kueue-sr-update", "10750506911" );
}

#include "kueueapp.moc"

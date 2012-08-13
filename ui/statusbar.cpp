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

#include "statusbar.h"
#include "settings.h"
#include "kueuethreads.h"

StatusBar* StatusBar::instance = 0;

StatusBar& StatusBar::getInstance()
{
    if ( !instance )
    {
        instance = new StatusBar;
    }
 
    return *instance; 
}

void StatusBar::destroy()
{
    if ( instance )
    {
        delete instance;
    }
      
    instance = 0;
}

StatusBar::StatusBar()
{
    mDownloadButton = new QToolButton( this );
    mDownloadButton->setGeometry( 0, 0, 20, 20 );
    mDownloadButton->setIcon( QIcon(":/icons/menus/download.png"));
    
    mThreadButton = new QToolButton( this );
    mThreadButton->setGeometry( 0, 0, 20, 20 );
    mThreadButton->setIcon( QIcon(":/icons/menus/up.png"));
    
    mDownloadManager = new DownloadManager( window() );

    connect( mDownloadButton, SIGNAL( clicked(bool) ), 
             this, SLOT( toggleDownloadManager()) );
    
    connect( mThreadButton, SIGNAL( clicked( bool ) ),
             this, SLOT( toggleThreadWidget() ) );
    
    connect( mDownloadManager, SIGNAL( downloadFinished() ),
             this, SLOT( popupDownloadManager() ) );
    
    setMaximumHeight( 20 );
    
    QFont font;
    font.setPixelSize( 12 );
    
    mProgress = new QProgressBar( this );
    mProgress->setMaximumHeight( 20 );
    mProgress->setMaximumWidth( 100 );
    mProgress->setFont( font );
    
    mLabel = new QLabel( this );
    mLabel->setMaximumHeight( 20 );
    mLabel->setFont( font );
    mLabel->setText( "" );

    addWidget( mThreadButton, 0 );
    addWidget( mProgress, 1 );
    addWidget( mLabel, 2 );
        
    addPermanentWidget( mDownloadButton );
    
    mProgress->hide();   
    mThreadButton->hide();
}

StatusBar::~StatusBar()
{
}

void StatusBar::toggleDownloadManager()
{
    if ( mDownloadManager->isHidden() )
    {
        showDownloadManager();
    }
    else
    {
        mDownloadManager->hide();
    }
}

void StatusBar::toggleThreadWidget()
{
    QWidget* tw = KueueThreads::threadWidget();
    
    if ( tw->isHidden() )
    {
        showThreadWidget();
    }
    else
    {
        tw->hide();
    }
}

void StatusBar::mousePressEvent( QMouseEvent* e )
{
    hideDownloadManager();
    QStatusBar::mousePressEvent( e );
}

void StatusBar::showDownloadManager()
{
    mDownloadManager->setGeometry( window()->x() + window()->width() - mDownloadManager->width(), window()->y() + window()->height() - mDownloadManager->height(), 
                                    ( window()->width() / 3 ), ( window()->height() / 4 * 3 ) );
    
    mDownloadManager->move( window()->x() + window()->width() - mDownloadManager->width(), 
                            window()->y() + window()->height() - mDownloadManager->height() );
    
    mDownloadManager->show();   
}

void StatusBar::showThreadWidget()
{
    QWidget* tw = KueueThreads::threadWidget();
    
    //tw->setGeometry( window()->x() + window()->width() - mDownloadManager->width(), window()->y() + window()->height() - mDownloadManager->height(), 
     //                               ( window()->width() / 3 ), ( window()->height() / 4 * 3 ) );
    
    tw->move( window()->x() + 5, window()->y() + window()->height() - tw->height() );
    
    tw->show();   
}


void StatusBar::hideDownloadManagerImpl()
{
    mDownloadManager->hide();
}

void StatusBar::popupDownloadManager()
{
    if ( Settings::showDownloadManager() )
    {
        showDownloadManager();
    }
}

void StatusBar::addDownloadJobImpl( QNetworkReply* reply, QString dir, bool ask )
{
    mDownloadManager->handleUnsupportedContent( reply, dir, ask );
}

void StatusBar::addDownloadJobImpl( QNetworkRequest req, QNetworkAccessManager* nam, QString dir, bool ask )
{
    mDownloadManager->download( req, nam, dir, ask );  
}

void StatusBar::showMessageImpl( QString message, int timeout )
{
    QTimer* t = new QTimer; 
    t->setInterval( timeout );
    t->setSingleShot( true );
    
    connect( t, SIGNAL( timeout() ), 
             this, SLOT( resetStatusBarImpl() ) );
    
    mLabel->setText( message );
    t->start();
}

void StatusBar::resetStatusBarImpl()
{
    mProgress->hide();
    mThreadButton->hide();
    mLabel->setText( "" );
}

void StatusBar::updateProgressImpl( int p, const QString& text )
{
    if ( !text.isNull() )
    {
        mLabel->setText( text );
    }

    mProgress->setValue( p );
}

void StatusBar::undefinedDownload()
{
    mLabel->setText( "Downloading..." );
    mProgress->setMinimum( 0 );
    mProgress->setMaximum( 0 );
    mProgress->show();
    mThreadButton->show();
}

void StatusBar::startJobStatusImpl( const QString& status, int total )
{
    mLabel->setText( status );
    mProgress->setMinimum( 0 );
    mProgress->setMaximum( total );
    mProgress->show();
    mThreadButton->show();
}

#include "statusbar.moc"

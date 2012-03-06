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

#include "helpbrowser.h"

#include <QDebug>
#include <QtWebKit>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QtNetwork>

HelpBrowser::HelpBrowser( QObject* parent )
{
    qDebug() << "[HELPBROWSER] Constructing";
    
    setupUi( this ); 
    
    QDir cacheDir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/cache" );
        
    QDesktopWidget* w = QApplication::desktop();
    int h = w->height();
    int res = ( ( h / 100 ) * 95 );
    
    QNetworkDiskCache *diskCache = new QNetworkDiskCache( this );
    diskCache->setCacheDirectory( cacheDir.absolutePath() );
    
    mWebView = new QWebView( this );
    mWebView->page()->networkAccessManager()->setCache( diskCache );
    mWebView->setUrl( QUrl( "http://kueue.hwlab.suse.de/doc" ) );
    
    int nw = ( mWebView->page()->settings()->fontSize(QWebSettings::DefaultFontSize) * 50 + 100 );
    resize( ( nw + ( nw / 12 ) ), res );

    browserLayout->addWidget( mWebView );
    
    connect( closeButton, SIGNAL( clicked() ), 
             this, SIGNAL( rejected() ) );
    
    connect( backButton, SIGNAL( clicked() ),
             this, SLOT( goBack() ) );
}

HelpBrowser::~HelpBrowser()
{
    qDebug() << "[HELPBROWSER] Destroying";
}

void HelpBrowser::closeEvent( QCloseEvent* event )
{
    emit rejected();
    QDialog::closeEvent( event );
}

void HelpBrowser::goBack()
{
    mWebView->triggerPageAction( QWebPage::Back );
}

#include "helpbrowser.moc"

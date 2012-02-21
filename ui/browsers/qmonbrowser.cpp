/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>

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

#include "qmonbrowser.h"
#include "settings.h"
#include "kueue.h"
#include "data/database.h"
#include "ui/html.h"
#include "ui/tabwidget.h"

#include <QMessageBox>
#include <QMenu>
#include <QWebFrame>
#include <QWidgetAction>
#include <QWebInspector>
#include <QShortcut>
#include <QGridLayout>
#include <QToolButton>
#include <QWebElementCollection>
#include <QNetworkReply>
#include <QDebug>


QMonBrowser::QMonBrowser( QObject *parent )
        : QWebView( ( QWidget* ) 0 )
{
    qDebug() << "[QMONBROWSER] Constructing";
    
    (void)parent;
    
    mFilter = QString::Null();
    
    connect( page(), SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ), 
             this, SLOT( urlHovered( const QString&, const QString&, const QString& ) ) );
        
    QShortcut* shortcut = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_I ), this );
   
    connect( shortcut, SIGNAL( activated() ), 
             this, SLOT( openWebInspector() ) );
    
    QShortcut* emea = new QShortcut( QKeySequence( Qt::Key_F1 ), this );
    emea->setObjectName( "emea" );
    
    connect( emea, SIGNAL( activated() ), 
             this, SLOT( setFilter() ) );
    
    QShortcut* us = new QShortcut( QKeySequence( Qt::Key_F2 ), this );
    us->setObjectName( "us" );
    
    connect( us, SIGNAL( activated() ), 
             this, SLOT( setFilter() ) );
    
    QShortcut* apac = new QShortcut( QKeySequence( Qt::Key_F3 ), this );
    apac->setObjectName( "apac" );
    
    connect( apac, SIGNAL( activated() ), 
             this, SLOT( setFilter() ) );
    
    QShortcut* all = new QShortcut( QKeySequence( Qt::Key_F4 ), this );
    all->setObjectName( "all" );
    
    connect( all, SIGNAL( activated() ), 
             this, SLOT( setFilter() ) );
    
    update();
}

QMonBrowser::~QMonBrowser()
{
    qDebug() << "[QMONBROWSER] Destroying";
}

void QMonBrowser::openWebInspector()
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    
    QWidget* w = new QWidget;
    QGridLayout* l = new QGridLayout;
    w->setLayout( l );
    QWebInspector* i = new QWebInspector( w );
    l->addWidget( i );
    i->setPage( page() );
    w->setWindowTitle( "Webinspector - qmonbrowser" );
    w->show();
}

void QMonBrowser::urlHovered( const QString& url, const QString& title, const QString& text )
{
    // Copy the last hovered URL to mSr
    // Necessary to make the mouseButtonEvents work on links
    
    (void)title;
    (void)text;
    
    mUrl = url;
}

void QMonBrowser::update()
{
    QStringList list = Settings::queuesToMonitor();
    QString html;
    
    qDebug() << "[QMONBROWSER] Updating";
    
    html += HTML::styleSheet();
    
    html += HTML::qmonPageHeader();
    
    for ( int i = 0; i < list.size(); ++i )
    {
        if ( list.at( i ).split( "|" ).at( 0 ).isEmpty() )
        {
            html += HTML::qmonTableHeader( list.at( i ).split( "|" ).at( 1 ) );
        }
        else
        {
            html += HTML::qmonTableHeader( list.at( i ).split( "|" ).at( 0 ) );
        }
        
        QList< SiebelItem* > l( Database::getSrsForQueue( list.at( i ).split( "|" ).at( 1 ), mFilter ) );
    
        for ( int i = 0; i < l.size(); ++i ) 
        {
            html += HTML::qmonSrInQueue( l.at( i ) );
            delete l.at( i );
        }
        
        html += HTML::qmonTableFooter();
    }
    
    QPoint pos = page()->currentFrame()->scrollPosition();
    page()->currentFrame()->setHtml( html );
    page()->currentFrame()->setScrollPosition( pos );
}

void QMonBrowser::mousePressEvent( QMouseEvent* event )
{
    // Left mouse button 
    // Different actions for different "protocols"
    
    if ( ( event->button() == 1 ) && 
         ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::leftMouseButton() == 0 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::leftMouseButton() == 1 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::leftMouseButton() == 2 )
        {
            toggleSrTable( mUrl.toString().remove( "sr://" ) );
        }
    }
        
    if ( ( event->button() == 1 ) &&
         ( mUrl.toString().startsWith(  "arrow://" ) ) )
    {
        toggleSrTable( mUrl.toString().remove( "arrow://" ) );
    }
    
    // Right mouse button     
    
    if ( ( event->button() == 2 ) && ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::rightMouseButton() == 0 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::rightMouseButton() == 1 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::rightMouseButton() == 2 )
        {
            toggleSrTable( mUrl.toString().remove( "sr://" ) );
        }
    }
    
    // Middle mouse button 
  
    if ( ( event->button() == 4 ) && 
         ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::middleMouseButton() == 0 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::middleMouseButton() == 1 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::middleMouseButton() == 2 )
        {
            toggleSrTable( mUrl.toString().remove( "sr://" ) );
        }
    }
    
    return QWebView::mousePressEvent(event);
}

void QMonBrowser::contextMenu( QMouseEvent* event, const QString& id )
{
    // Create a custom context menu plus a widget to hold the actual menu
    // This makes it look more like a KMenu (way more beautiful)
    
    QMenu* menu = new QMenu();
            
    QAction* ba = new QAction( menu );
    QAction* oc = new QAction( "Show/Hide SR details", menu );
    QAction* dd = new QAction( "Show detailed description", menu );
    QAction* cb = new QAction( "Copy SR# to clipboard", menu );
    QAction* ts = new QAction( "Take SR and copy SR# to clipboard", menu );
    QAction* ou = new QAction( "Show in Unity", menu );
    QWidgetAction* wa = new QWidgetAction( menu );
    
    QFont font = ba->font();
    font.setBold( true );
    
    QToolButton* titleButton = new QToolButton( this );
    
    wa->setDefaultWidget( titleButton );
    wa->setObjectName( "MENUTITLE" );
    
    titleButton->setDefaultAction( ba );
    titleButton->setDown( true );
    titleButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    
    ba->setFont( font );
    ba->setIcon( QIcon( QIcon( ":/icons/kueue.png" ).pixmap( QSize( 16, 16 ) ) ) );
    ba->setText( id );
    
    oc->setData( id + "|tog" );
    dd->setData( id + "|dd" );
    cb->setData( id + "|cb" );
    ts->setData( id + "|ts" );
    ou->setData( id + "|ou" );
    
    oc->setIcon( QIcon( ":/icons/menus/toggle.png" ) );
    dd->setIcon( QIcon( ":/icons/menus/ddesc.png" ) );
    cb->setIcon( QIcon( ":/icons/menus/clipboard.png" ) );
    ts->setIcon( QIcon( ":/icons/menus/take.png" ) );
    ou->setIcon( QIcon( ":/icons/menus/siebel.png" ) );
    
    menu->addAction( wa );
    menu->addAction( oc );
    menu->addSeparator();
    menu->addAction( dd );
    menu->addSeparator();
    menu->addAction( cb );
    menu->addSeparator();
    menu->addAction( ts );
    
    if ( Settings::unityEnabled() )
    {
        menu->addSeparator();
        menu->addAction( ou );
    }
    
    connect( menu, SIGNAL( triggered( QAction* ) ), 
             this, SLOT( contextMenuItemTriggered( QAction* ) ) );
    
    menu->exec( event->globalPos() );
    
    delete menu;
}

void QMonBrowser::contextMenuEvent(QContextMenuEvent* event)
{
    // Ignore the contextMenuEvent
    // This is necessary to make the right button events work
    
    event->ignore();
}

void QMonBrowser::contextMenuItemTriggered( QAction* a )
{   
    // This is called when a QAction from within the contextMenu is clicked
    // The data.contains thing is an ugly hack... But it works
    
    if ( a->data().toString().contains( "|cb" ) )
    {
        Kueue::setClipboard( a->data().toString().remove( "|cb" ) );
    }
    else if ( a->data().toString().contains( "|tog" ) )
    {
        toggleSrTable( a->data().toString().remove( "|tog" ) );
    }
    else if ( a->data().toString().contains( "|dd" ) )
    {
        Kueue::showDetailedDescription( a->data().toString().remove( "|dd" ), false );
    }
    else if ( a->data().toString().contains( "|ts" ) )
    {
        takeSR( a->data().toString().remove( "|ts" ) );
    }
    else if ( a->data().toString().contains( "|ou" ) )
    {
        TabWidget::openInUnity( a->data().toString().remove( "|ou" ) );
    }

}

void QMonBrowser::toggleSrTable( const QString& id )
{
    // Toggle the detailed SR infos table 
    
    QWebElementCollection list = page()->mainFrame()->findAllElements( "table" );
    
    for ( int i = 0; i < list.count(); ++i ) 
    {
        if ( list.at(i).attribute( "id" ) == id + "_body" ) 
        { 
            if ( list.at(i).attribute( "style" ) == "display:none" ) 
            {
                list.at( i ).setAttribute( "style", "display:block" );
                Database::updateSiebelDisplay( id + "-block" );
            }
            else if ( list.at(i).attribute( "style" ) == "display:block" ) 
            {
                list.at( i ).setAttribute("style", "display:none" );
                Database::updateSiebelDisplay( id + "-none" );
            }
        }
    }
}

void QMonBrowser::takeSR( const QString& sr )
{
    QMessageBox* box = new QMessageBox( this );
    
    box->setText( "Assign <b>" + sr + "</b> to " + Settings::engineer() + "?" );
    box->setWindowTitle( "Assign SR" );
    box->setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    box->setDefaultButton( QMessageBox::Yes );
    box->setIcon( QMessageBox::Question );
    
    int reply = box->exec();
    
    if ( reply == QMessageBox::Yes ) 
    {
        showProgress();
        mAssign = Kueue::download( QUrl( Settings::dBServer() + "/assign.asp?sr=" + sr + "&owner=" + Settings::engineer().toUpper() ) );
        connect( mAssign, SIGNAL( finished() ), this, SLOT( assignFinished() ) );
    }
    
    delete box;
}

void QMonBrowser::showProgress()
{
    mProgress = new QProgressDialog( this );
    mProgress->setWindowTitle( "Assigning SR" );
    mProgress->setLabelText( "Assigning SR to " + Settings::engineer().toUpper() );
    mProgress->setMinimum( 0 );
    mProgress->setMaximum( 0 );
    mProgress->setWindowModality( Qt::WindowModal );
    mProgress->show();
}

void QMonBrowser::assignFinished()
{
    QString data;
    
    mProgress->close();
    
    if( mAssign->error() )
    {
            QMessageBox::critical( this, "Error", "Error sending data to Server" );
            qDebug() << "[QMONBROWSER] Assign error: " << mAssign->errorString();
    }
    else
    {
        qDebug() << "[QMONBROWSER] Assign success: " << mAssign->errorString();
        data = mAssign->readAll();
    }

    if ( data.startsWith( "ASSIGNED" ) )
        QMessageBox::critical( this, "Error", "SR already assigned to " + data.split( "|" ).at( 2 ) );

    else if ( data.startsWith( "SR_NOT_FOUND" ) )
        QMessageBox::critical( this, "Error", "SR not found");

    else if ( data.startsWith( "NO_SR" ) )
        QMessageBox::critical( this, "Error", "No SR number specified" );

    else if ( data.startsWith( "NO_OWNER" ) )
        QMessageBox::critical( this, "Error", "No assignee specified" );

    else if ( data.startsWith( "FAILURE" ) )
        QMessageBox::critical( this, "Error", "General failure" );

    else if ( data.startsWith( "SUCCESS" ) )
        QMessageBox::information( this, "Done", "SR successfully assigned to " + Settings::engineer() );
    
    else QMessageBox::critical( this, "Error", "Unknown reply: " + data );
    
    return;
}

void QMonBrowser::setFilter()
{
    QShortcut* sc = qobject_cast<QShortcut*>( sender() );
    
    if ( sc->objectName() == "emea" )
    {
        mFilter = "EMEA";
    }
    else if ( sc->objectName() == "us" )
    {
        mFilter = "USA";
    }
    else if ( sc->objectName() == "apac" )
    {
        mFilter  = "APAC";
    }
    else
    {
        mFilter = QString::Null();
    }
    
    update();
}


#include "qmonbrowser.moc"

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

#include "statsbrowser.h"
#include "settings.h"
#include "ui/html.h"
#include "kueue.h"

#include <QDebug>
#include <QMenu>
#include <QWidgetAction>
#include <QToolButton>
#include <QWebElementCollection>
#include <QWebFrame>

StatsBrowser::StatsBrowser(QObject* parent)
        : Browser( ( QWidget* ) 0 )
{
    qDebug() << "[STATSBROWSER] Constructing";
    
    connect( page(), SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
             this, SLOT( urlHovered( const QString&, const QString&, const QString& ) ) );    
}

StatsBrowser::~StatsBrowser()
{
    qDebug() << "[STATSBROWSER] Destroying";
}

void StatsBrowser::update( const QString& html )
{
    //qDebug() << "[STATSBROWSER] Updating";

    QPoint pos = page()->currentFrame()->scrollPosition();
    page()->currentFrame()->setHtml( html );
    page()->currentFrame()->setScrollPosition( pos );
}

void StatsBrowser::filter( const QString& filter )
{
}

void StatsBrowser::urlHovered( const QString& url, const QString& title, const QString& text )
{
    // Copy the last hovered URL to mSr
    // Necessary to make the mouseButtonEvents work on links
    
    (void)title;
    (void)text;
    
    mUrl = url;
}

void StatsBrowser::mousePressEvent( QMouseEvent* event )
{
    StatusBar::hideDownloadManager();
    
    if ( ( event->button() == 1 ) && 
         ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::leftMouseButton() == 1 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::leftMouseButton() == 2 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::leftMouseButton() == 0 )
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
    
    if ( ( event->button() == 2 ) && 
         ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::rightMouseButton() == 1 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::rightMouseButton() == 2 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::rightMouseButton() == 0 )
        {
            toggleSrTable( mUrl.toString().remove( "sr://" ) );
        }
    }
    
    // Middle mouse button 
  
    if ( ( event->button() == 4 ) && 
         ( mUrl.toString().startsWith(  "sr://" ) ) )
    {
        if ( Settings::middleMouseButton() == 1 )
        {
            contextMenu( event, mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::middleMouseButton() == 2 )
        {
            Kueue::setClipboard( mUrl.toString().remove( "sr://" ) );
        }
        else if ( Settings::middleMouseButton() == 0 )
        {
            toggleSrTable( mUrl.toString().remove( "sr://" ) );
        }
    }
    
    return QWebView::mousePressEvent(event);
}

void StatsBrowser::contextMenu( QMouseEvent* event, const QString& id )
{
    QMenu* menu = new QMenu();
            
    QAction* ba = new QAction( menu );
    QAction* dd = new QAction( "Show detailed description", menu );
    QAction* cb = new QAction( "Copy SR# to clipboard", menu );
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
    ba->setIcon( QIcon( ":/icons/kueue.png" ).pixmap( QSize( 16, 16 ) ) );
    ba->setText( id );
    
    dd->setData( id + "|dd" );
    cb->setData( id + "|cb" );
    
    dd->setIcon( QIcon( ":/icons/menus/ddesc.png" ) );
    cb->setIcon( QIcon( ":/icons/menus/clipboard.png" ) );
    
    menu->addAction( wa );
    menu->addAction( dd );
    menu->addSeparator();
    menu->addAction( cb );
    
    connect( menu, SIGNAL( triggered( QAction* ) ), 
             this, SLOT( contextMenuItemTriggered( QAction* ) ) );
    
    menu->exec( event->globalPos() );
    
    delete menu;
}

void StatsBrowser::contextMenuEvent(QContextMenuEvent* event)
{
    // Ignore the contextMenuEvent
    // This is necessary to make the right button events work
    
    event->ignore();
}

void StatsBrowser::contextMenuItemTriggered( QAction* a )
{   
    // This is called when a QAction from within the contextMenu is clicked
    // The data.contains thing is an ugly hack... But it works
    
    if ( a->data().toString().contains( "|cb" ) )
    {
        Kueue::setClipboard( a->data().toString().remove( "|cb" ) );
    }
    else if ( a->data().toString().contains( "|dd" ) )
    {
        Kueue::showDetailedDescription( a->data().toString().remove( "|dd" ), true );
    }
}

void StatsBrowser::toggleSrTable( const QString& id )
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
            }
            else if ( list.at(i).attribute( "style" ) == "display:block" ) 
            {
                list.at( i ).setAttribute("style", "display:none" );
            }
        }
    }
}

#include "statsbrowser.moc"

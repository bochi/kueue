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

#include "queue.h"
#include "data/database.h"
#include "data/sr.h"
#include "network.h"
#include "ui/kueuewindow.h"
#include "kueue.h"
#include "ui/html.h"

#include <QDebug>
#include <QShortcut>
#include <QGridLayout>
#include <QWebInspector>
#include <QWebFrame>
#include <QMenu>
#include <QWidgetAction>
#include <QWebElementCollection>
#include <QToolButton>


Queue::Queue( QObject* parent )
{
    qDebug() << "[QUEUE] Constructing";

    mTimer = new QTimer( this );

    connect( mTimer, SIGNAL( timeout() ),
             this, SLOT( update() ) );

    mReply = Kueue::download( QUrl( Settings::dBServer() + "/open.asp?tse=" + Settings::engineer() ) );
   
    connect( mReply, SIGNAL( finished() ),
             this, SLOT( jobDone() ) );
            
    mTimer->start( Settings::autoMinutes() * 60000 );
}

Queue::~Queue()
{
    qDebug() << "[QUEUE] Destroying";
    
    if ( mTimer != 0 )
    {
        mTimer->stop();
        delete mTimer;
        mTimer = 0;
    }
    
    if ( mReply->isRunning() )
    {
        mReply->abort();
    }
}

void Queue::update()
{
    if ( !mReply->isRunning() )
    {
        mReply = Kueue::download( QUrl( Settings::dBServer() + "/open.asp?tse=" + Settings::engineer() ) );
        
        connect( mReply, SIGNAL( finished() ),
                 this, SLOT( jobDone() ) );
    }
    else
    {
        qDebug() << "[QUEUE] Update still running - skipping";
    }
}

void Queue::jobDone()
{
    bool changed = false;
    bool initial = false;
    QString replydata = mReply->readAll();
    
    if ( !mReply->error() )
    {
        replydata.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
    
        QRegExp srnr( "^[0-9]{11}$" );
        QStringList dblist( Database::getSrList() );
        
        if ( dblist.isEmpty() )
        {
            initial = true;
        }
        
        QStringList list = replydata.split( Settings::engineer().toUpper() + "|" );
        QStringList newlist;

        for ( int i = 0; i < list.size(); ++i ) 
        {
            if ( srnr.exactMatch( list.at( i ).split( "|" ).at( 0 ) ) ) 
            {
                newlist.append( list.at( i ).split( "|" ).at( 0 ) );
            }
        }
        
        for ( int i = 0; i < dblist.size(); ++i ) 
        {
            if ( srnr.exactMatch( dblist.at( i ) ) )
            {
                if ( !( newlist.contains( dblist.at( i ) ) ) ) 
                {
                    Database::deleteSrFromDB( dblist.at( i ) ); 
                    changed = true;
                }
            }
        }
    
        if ( initial ) 
        {
            emit initialUpdate( list.size(), 2 );
        }
            
        for ( int i = 0; i < list.size(); ++i ) 
        {
            if ( srnr.exactMatch( list.at( i ).split( "|" ).at( 0 ) ) ) 
            {
                QString id = list.at( i ).split( "|" ).at( 0 );
                QString adate = list.at( i ).split( "|" ).at( 1 );
                QString status = list.at( i ).split( "|" ).at( 2 );

                if ( ( Database::convertTime( adate ) != Database::getAdate( id ) ) || 
                     ( Database::getStatus( id ) != status ) )
                {
                    QString srdata;
                    
                    QNetworkReply *reply = Kueue::download( QUrl( Settings::dBServer() + "/stefan.asp?sr=" + id ) );
                    QEventLoop loop;
 
                    QObject::connect( reply, SIGNAL( finished() ), 
                                      &loop, SLOT( quit() ) );
                   
                    loop.exec();
                         
                    srdata = reply->readAll();
                    srdata.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );

                    if ( srdata.contains( "|||" ) )
                    {
                        SR* sr = new SR( id );
                        QDateTime o = QDateTime::fromString( srdata.split("|||").at( 1 ).trimmed(), "M/d/yyyy h:mm:ss AP" );
                        QDateTime a = QDateTime::fromString( srdata.split("|||").at( 2 ).trimmed(), "M/d/yyyy h:mm:ss AP" );

                        sr->setId( id );
                        sr->setOpened( o );
                        sr->setLastUpdate( a );
                        sr->setStatus( srdata.split("|||").at( 3 ).trimmed() );
                        sr->setCustomer( srdata.split("|||").at( 4 ).trimmed() );
                        sr->setContact( srdata.split("|||").at( 5 ).trimmed() );
                        sr->setBriefDesc( srdata.split("|||").at( 6 ).trimmed() );
                        sr->setDetailedDesc( srdata.split("|||").at( 7 ).trimmed() );
                        
                        if ( srdata.split("|||").at( 0 ).trimmed() == "no" )
                        {
                            sr->setSs( false );
                        }
                        else
                        {
                            sr->setSs( true );
                        }
                        
                        if ( Database::srExistsInDB( sr->id() ) )
                        {
                            Database::updateSRData( sr );
                            
                            if ( !initial )
                            {
                                Kueue::notify( "kueue-sr-update", "SR Updated", "<b>" + sr->id() + 
                                                   "</b><br>" + sr->briefDesc(), sr->id() );
                                
                                if ( Settings::animateQueue() ) 
                                {
                                    Kueue::attention( true );
                                }
                            }

                            changed = true;
                        }
                        else
                        {   
                            Database::insertSRData( sr );
                            
                            if ( !initial )
                            {
                                Kueue::notify( "kueue-sr-new", "New SR in your queue", "<b>" + id +
                                                   "</b><br>" + sr->briefDesc(), sr->id() );
                                
                                if ( Settings::animateQueue() ) 
                                {
                                    Kueue::attention( true );
                                }
                            }
                            
                            changed = true;
                        }
                        
                        delete sr;
                    }
                    
                    reply->deleteLater();
                }
            }
            
            if ( initial ) emit initialUpdateProgress( i );
        }
        
        if ( initial ) emit initialUpdateDone();
    
        if ( changed )
        {
            emit queueDataChanged();
        }
    }
}


#include "queue.moc"

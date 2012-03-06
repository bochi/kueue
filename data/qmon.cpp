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

#include "qmon.h"
#include "data/database.h"
#include "settings.h"
#include "network.h"
#include "kueue.h"
#include "ui/html.h"

#include <QMessageBox>
#include <QMenu>
#include <QWebFrame>
#include <QWidgetAction>
#include <QWebInspector>
#include <QShortcut>
#include <QGridLayout>
#include <QToolButton>
#include <QWebElementCollection>
#include <QtXml>

Qmon::Qmon()
{
    qDebug() << "[MONITOR] Constructing";
    
    mTimer = new QTimer( this );
    
    connect( mTimer, SIGNAL( timeout() ),
             this, SLOT( update() ) );

    if ( Settings::monitorEnabled() )
    {
        QUrl url( "http://kueue.hwlab.suse.de/qmon" );
        url.setPort( 8080 );

        mSiebelReply = Kueue::download( url );
        
        connect( mSiebelReply, SIGNAL( finished() ),
                 this, SLOT( siebelJobDone() ) );
        
        mTimer->start( Settings::monitorMinutes() * 60000 );
    }
    
    QStringList queueList = Settings::queuesToMonitor();
    
    for ( int i = 0; i < queueList.size(); ++i ) 
    {
        mQueueList.append( queueList.at( i ).split( "|" ).at( 1 ) );
    }
}

Qmon::~Qmon()
{
    qDebug() << "[MONITOR] Destroying";
    
    mQueueList.clear();
    
    if ( mTimer != 0 )
    {
        mTimer->stop();
        delete mTimer;
        mTimer = 0;
    }
}

void Qmon::update()
{
    if ( !mSiebelReply->isRunning() ) 
    {
        QUrl url( "http://kueue.hwlab.suse.de/qmon" );
        url.setPort( 8080 );
        
        mSiebelReply = Kueue::download( url );
        
        connect( mSiebelReply, SIGNAL( finished() ), 
                 this, SLOT( siebelJobDone() ) );
    }
    else
    {
        qDebug() << "[MONITOR] Siebel update still running - skipping";
    }
}

void Qmon::siebelJobDone()
{
    QString replydata = mSiebelReply->readAll();
    bool initial = false;
    
    QDomDocument doc;
    QDomNodeList list;
    QStringList newList;
    QStringList existList = Database::getQmonSiebelList();
    
    if ( existList.isEmpty() )
    {
        initial = true;
    }
        
    if ( !mSiebelReply->error() )
    {
        doc.setContent( replydata );
        list = doc.elementsByTagName( "sr" );
        
        if ( initial ) emit initialUpdate( list.size(), 1u );
        
        for ( int i = 0; i < list.size(); ++i ) 
        {    
            SiebelItem* si = new SiebelItem;
                
            si->id = list.at( i ).namedItem( "id" ).toElement().text(); 
            si->queue = list.at( i ).namedItem( "queue" ).toElement().text(); 
            si->severity = list.at( i ).namedItem( "severity" ).toElement().text(); 
            si->status = list.at( i ).namedItem( "status" ).toElement().text(); 
            si->bdesc = list.at( i ).namedItem( "bdesc" ).toElement().text(); 
            si->geo = list.at( i ).namedItem( "geo" ).toElement().text(); 
            si->hours = list.at( i ).namedItem( "hours" ).toElement().text(); 
            si->customer = list.at( i ).namedItem( "customer" ).toElement().text(); 
            si->contactvia = list.at( i ).namedItem( "contactvia" ).toElement().text(); 
            si->contract = list.at( i ).namedItem( "contract" ).toElement().text(); 
            
            if ( list.at( i ).namedItem( "srtype" ).toElement().text() == "sr" )
            {
                si->isCr = false;
            }
            else
            {
                si->isCr = true;
                si->creator = list.at( i ).namedItem( "creator" ).toElement().text(); 
            }
            
            if ( list.at( i ).namedItem( "bomgarQ" ).toElement().text().isEmpty() )
            {
                si->isChat = false;
            }
            else
            {
                si->isChat = true;
                si->bomgarQ = list.at( i ).namedItem( "bomgarQ" ).toElement().text(); 
            }
            
            if ( list.at( i ).namedItem( "highvalue" ).toElement().text() == "yes" )
            {
                si->highValue = true;
            }
            else
            {
                si->highValue = false;
            }
            
            if ( list.at( i ).namedItem( "critsit" ).toElement().text() == "yes" )
            {
                si->critSit = true;
            }
            else
            {
                si->critSit = false;
            }

            si->age = list.at( i ).namedItem( "age" ).toElement().text().toInt(); 
            si->lastAct = list.at( i ).namedItem( "lastupdate" ).toElement().text().toInt(); 
            si->timeInQ = list.at( i ).namedItem( "timeinQ" ).toElement().text().toInt(); 
            si->slaLeft = list.at( i ).namedItem( "sla" ).toElement().text().toInt(); 
            
            newList.append( si->id );
    
            if ( !Database::siebelExistsInDB( si->id ) )
            {                    
                Database::insertSiebelItemIntoDB( si );
                
                if ( !initial && mQueueList.contains( si->queue ) )
                {
                    if ( si->severity == "Low" )
                    {               
                        Kueue::notify( "kueue-monitor-low", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "Medium" )
                    {
                        Kueue::notify( "kueue-monitor-medium", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "Urgent" )
                    {
                        Kueue::notify( "kueue-monitor-urgent", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "High" )
                    {
                        Kueue::notify( "kueue-monitor-high", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }

                    if ( Settings::animateQmon() ) 
                    {
                        Kueue::attention( true );
                    }
                }
        
             /*   if ( Database::siebelQueueChanged( si ) )
                {
                    qDebug() << "[MONITOR] Siebel Queue Changed for SR" << si->id;
                    Database::updateSiebelQueue( si );
                }
                
                if ( Database::siebelSeverityChanged( si ) )
                {
                    qDebug() << "[MONITOR] Siebel Severity Changed for SR" << si->id;
                    Database::updateSiebelSeverity( si );
                }*/
            }
            else
            {
                if ( ( Database::siebelQueueChanged( si ) ) &&
                     ( mQueueList.contains( si->queue ) ) )
                {
                    if ( si->severity == "Low" )
                    {               
                        Kueue::notify( "kueue-monitor-low", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "Medium" )
                    {
                        Kueue::notify( "kueue-monitor-medium", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "Urgent" )
                    {
                        Kueue::notify( "kueue-monitor-urgent", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }
                    else if ( si->severity == "High" )
                    {
                        Kueue::notify( "kueue-monitor-high", "New SR in " + QString( si->queue ), 
                                            "<b>SR#" + si->id + "</b><br>" + si->bdesc, si->id );
                    }

                    if ( Settings::animateQmon() ) 
                    {
                        Kueue::attention( true );
                    }
                }
                
                if ( ( Database::siebelSeverityChanged( si ) ) &&
                     ( mQueueList.contains( si->queue ) ) )
                {
                    if ( si->severity == "Low" )
                    {               
                        Kueue::notify( "kueue-monitor-low", "Severity for <b>SR#" + si->id + "</b> changed", 
                                                            "Severity is now<b> " + si->severity + "</b>", si->id );
                    }
                    else if ( si->severity == "Medium" )
                    {
                        Kueue::notify( "kueue-monitor-medium", "Severity for <b>SR#" + si->id + "</b> changed", 
                                                            "Severity is now<b> " + si->severity + "</b>", si->id );
                    }
                    else if ( si->severity == "Urgent" )
                    {
                        Kueue::notify( "kueue-monitor-urgent", "Severity for <b>SR#" + si->id + "</b> changed", 
                                                            "Severity is now<b> " + si->severity + "</b>", si->id );
                    }
                    else if ( si->severity == "High" )
                    {
                        Kueue::notify( "kueue-monitor-high", "Severity for <b>SR#" + si->id + "</b> changed", 
                                                            "Severity is now<b> " + si->severity + "</b>", si->id );
                    }

                    if ( Settings::animateQmon() ) 
                    {
                        Kueue::attention( true );
                    }
                }
                
                Database::updateSiebelItemInDB( si );
            }
            
            delete si;
            emit initialUpdateProgress( i );
        }
        
        emit initialUpdateDone();
    
        for ( int i = 0; i < existList.size(); ++i ) 
        {
            if ( !newList.contains( existList.at( i ) ) )
            {
                Database::deleteSiebelItemFromDB( existList.at( i ) );
            }
        }
    }
    else
    {
        qDebug() << "[MONITOR] Siebel Error:" << mSiebelReply->errorString();
    }
    
    emit qmonDataChanged();
}


#include "qmon.moc"


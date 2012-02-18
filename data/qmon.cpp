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
            si->hours = list.at( i ).namedItem( "hours" ).toElement().text(); 
            si->source = list.at( i ).namedItem( "source" ).toElement().text(); 
            si->contactvia = list.at( i ).namedItem( "contactvia" ).toElement().text(); 
            si->openedSec = list.at( i ).namedItem( "age" ).toElement().text().toInt(); 
            si->activitySec = list.at( i ).namedItem( "lastupdate" ).toElement().text().toInt(); 
            si->queueSec = list.at( i ).namedItem( "timeinqueue" ).toElement().text().toInt();
            si->status = list.at( i ).namedItem( "status" ).toElement().text(); 
            si->contract = list.at( i ).namedItem( "contract" ).toElement().text(); 
            //si->queue1 = list.at( i ).namedItem( "" ).toElement().text(); 
            si->phone = list.at( i ).namedItem( "phone" ).toElement().text(); 
            //si->onsitephone = list.at( i ).namedItem( "" ).toElement().text(); 
            si->geo = list.at( i ).namedItem( "geo" ).toElement().text(); 
            //si->wtf = list.at( i ).namedItem( "" ).toElement().text(); 
            //si->routing = list.at( i ).namedItem( "" ).toElement().text(); 
            si->slaSec = list.at( i ).namedItem( "sla" ).toElement().text().toInt(); 
            si->bdesc = list.at( i ).namedItem( "description" ).toElement().text(); 
            si->bomgarQ = list.at( i ).namedItem( "bomgarQ" ).toElement().text();
            
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

void Qmon::bomgarJobDone()
{
    QString replydata = mBomgarReply->readAll();
    QStringList list;
    QStringList existList( Database::getQmonBomgarList() );
    bool changed = false;
    
    if ( !mBomgarReply->error() )
    {
        replydata.remove( QRegExp( "<(?:div|span|tr|td|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
    
        QStringList list = replydata.split( "<br>" );
        list.removeDuplicates();
    
        for ( int i = 0; i < list.size(); ++i ) 
        {
            if ( ( ( !list.at( i ).isEmpty() ) && 
                 ( list.at( i ).trimmed() != "no data" ) && 
                 ( list.at( i ).trimmed().contains( "|" ) ) ) )
            {
                BomgarItem* bi = new BomgarItem;
        
                bi->id = list.at( i ).trimmed().split( "|" ).at( 1 ).trimmed();
                bi->sr = list.at( i ).trimmed().split( "|" ).at( 2 ).trimmed();
                bi->repteam = list.at( i ).trimmed().split( "|" ).at( 3 ).trimmed();
                bi->name = list.at( i ).trimmed().split( "|" ).at( 4 ).trimmed();
                bi->date = list.at( i ).trimmed().split( "|" ).at( 5 ).trimmed();
                bi->someNumber = list.at( i ).trimmed().split( "|" ).at( 6 ).trimmed();
                
                list.append( bi->id );
        
                if ( !Database::bomgarExistsInDB( bi->id ) )
                {
                    Database::updateBomgarItemInDB( bi );
                    changed = true;
                }
                else if ( ( Database::getBomgarQueueById( bi->id ) != bi->name ) )
                {
                    Database::updateBomgarQueue( bi );
                    mNotifiedList.removeAll( bi->sr );
                    changed = true;
                }
                
                if ( ( Settings::monitorPersonalBomgar() ) && 
                     ( bi->name == Settings::bomgarName() ) && 
                     ( !mNotifiedList.contains( bi->sr ) ) )
                {
                    mNotifiedList.append( bi->sr );
                    Kueue::notify( "kueue-personal-bomgar", "Customer in Bomgar", "<b>SR#" + bi->sr + "</b>", "<br>" );
                    if ( Settings::animateQmon() ) Kueue::attention( true );
                }
                
                delete bi;
            }
        }
 
        for ( int i = 0; i < existList.size(); ++i ) 
        {
            if ( !list.contains( existList.at( i ) ) )
            {
                Database::deleteBomgarItemFromDB( existList.at( i ) );
                changed = true;
            }
        }
    }
    else
    {
        Kueue::notify( "kueue-general", "Error", "<b>Updating Bomgar Data failed.</b><br>No VPN connection or networking issues?", "" );
        qDebug() << "[MONITOR] Bomgar Error:" << mBomgarReply->errorString();
    }
    
    if ( changed )
    {
        emit qmonDataChanged();
    }
}


void Qmon::whoIsInBomgarJobDone( QNetworkReply* reply )
{
    QString replydata = reply->readAll();
    QString who;
    QString whoItem;
    QStringList whoList;
    
    if ( !reply->error() )
    {
        who = replydata.split( "<b>Time In System (min)</b></tr>" ).at(1);
        whoList = who.split( "<tr><tr><td><td>" );
        
        for ( int i = 0; i < whoList.size(); ++i ) 
        {
           WhoIsInBomgarItem* w = new WhoIsInBomgarItem;
           whoItem = whoList.at( i );
           whoItem.remove( "<tr><td><td>" );
           
           w->name = whoItem.split( "<td><td>" ).at( 0 );
           w->sr = whoItem.split( "<td><td>" ).at( 1 );
           w->timeInQueue = whoItem.split( "<td><td>" ).at( 2 );
           w->timeInSystem = whoItem.split( "<td><td>" ).at( 3 );
           
           qDebug() << "[MONITOR] New WhoIsInBomgarItem" << w->name << w->sr << w->timeInQueue << w->timeInSystem;
           
           delete w;
        }        
    }
}


#include "qmon.moc"


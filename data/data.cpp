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

#include "data.h"
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
#include <QDesktopServices>

Data::Data()
{
    qDebug() << "[DATA] Constructing, threadID" << thread()->currentThreadId();
    
    mNAM = new QNetworkAccessManager( this );
    
    mDB = "db-" + QString::number( thread()->currentThreadId() );
    
    QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
    
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", mDB );
    db.setDatabaseName( dir.path() + "/database.sqlite" );
    
    if ( !db.open() )
    {
        qDebug() << "[DATA] Failed to open the database " + mDB;
    }
    else
    {
        qDebug() << "[DATA] Opened DB" << dir.path() << mDB;
    }
    
    QTimer* queueTimer = new QTimer( this );
    queueTimer->start( 84726 );
    
    connect( queueTimer, SIGNAL( timeout() ),
             this, SLOT( updateQueue() ) );
    
    updateQueue();
    
    if ( Settings::monitorEnabled() )
    {
        QTimer* qmonTimer = new QTimer( this );
        qmonTimer->start( 58219 );
    
        connect( qmonTimer, SIGNAL( timeout() ),
                 this, SLOT( updateQmon() ) );
        
        updateQmon();
    }
    
    if ( Settings::statsEnabled() )
    {
        QTimer* statsTimer = new QTimer( this );
        statsTimer->start( 912913 );
    
        connect( statsTimer, SIGNAL( timeout() ),
                 this, SLOT( updateStats() ) );
        
        updateStats();
    }
}

Data::~Data()
{
    qDebug() << "[DATA] Destroying";
}

QNetworkReply* Data::get( const QUrl& url )
{
    QNetworkRequest request( url );
    request.setRawHeader( "User-Agent", QString( "kueue " + QApplication::applicationVersion() ).toUtf8() );
    
    QNetworkReply* reply = mNAM->get( request );
    
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( getError( QNetworkReply::NetworkError ) ) );

    return reply;
}

void Data::getError( QNetworkReply::NetworkError error )
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( QObject::sender() );
    
    qDebug() << "[NETWORK] Error getting" << reply->url();
}

void Data::updateQueue()
{
    QNetworkReply* r = get( QUrl( Settings::dBServer() + "/userqueue/" + Settings::engineer() ) );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( queueUpdateFinished() ) );
}

void Data::updateQmon()
{
    QNetworkReply* r = get( QUrl( Settings::dBServer() + "/qmon" ) );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( qmonUpdateFinished() ) );
}

void Data::updateStats()
{
    QNetworkReply* r = get( QUrl( Settings::dBServer() + "/stats/" + Settings::engineer() ) );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( statsUpdateFinished() ) );
}

void Data::queueUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
    QDateTime now = QDateTime::currentDateTime();
    QStringList existList = Database::getSrNrList( mDB );
    QStringList newList;

    QDomDocument doc;
    doc.setContent( r->readAll() );
    QDomNodeList list = doc.elementsByTagName( "sr" );
    
    QSqlDatabase::database( mDB ).transaction();
    
    for ( int i = 0; i < list.size(); ++i ) 
    {   
        QueueSR sr;
        
        sr.id = list.at( i ).namedItem( "id" ).toElement().text(); 
        sr.srtype = list.at( i ).namedItem( "srtype" ).toElement().text(); 
        sr.creator = list.at( i ).namedItem( "creator" ).toElement().text(); 
        sr.cus_account = list.at( i ).namedItem( "cus_account" ).toElement().text(); 
        sr.cus_firstname = list.at( i ).namedItem( "cus_firstname" ).toElement().text(); 
        sr.cus_lastname = list.at( i ).namedItem( "cus_lastname" ).toElement().text(); 
        sr.cus_title = list.at( i ).namedItem( "cus_title" ).toElement().text(); 
        sr.cus_email = list.at( i ).namedItem( "cus_email" ).toElement().text(); 
        sr.cus_phone = list.at( i ).namedItem( "cus_phone" ).toElement().text(); 
        sr.cus_onsitephone = list.at( i ).namedItem( "cus_onsitephone" ).toElement().text(); 
        sr.cus_lang = list.at( i ).namedItem( "cus_lang" ).toElement().text(); 
        sr.severity = list.at( i ).namedItem( "severity" ).toElement().text(); 
        sr.status = list.at( i ).namedItem( "status" ).toElement().text(); 
        sr.bdesc = list.at( i ).namedItem( "bdesc" ).toElement().text(); 
        sr.ddesc = list.at( i ).namedItem( "ddesc" ).toElement().text(); 
        sr.geo = list.at( i ).namedItem( "geo" ).toElement().text(); 
        sr.hours = list.at( i ).namedItem( "hours" ).toElement().text(); 
        sr.contract = list.at( i ).namedItem( "contract" ).toElement().text(); 
        sr.created = list.at( i ).namedItem( "created" ).toElement().text(); 
        sr.lastupdate = list.at( i ).namedItem( "lastupdate" ).toElement().text(); 
        sr.service_level = list.at( i ).namedItem( "service_level" ).toElement().text().toInt();
        sr.highvalue = list.at( i ).namedItem( "highvalue" ).toElement().text().toInt(); 
        sr.critsit = list.at( i ).namedItem( "critsit" ).toElement().text().toInt();
        
        if ( Database::srExistsInDB( sr.id, mDB ) )
        {
            Database::updateSRData( sr, mDB );
        }
        else
        {
            Database::insertSRData( sr, mDB );
        }
        
        newList.append( sr.id );
    }
    
    for ( int i = 0; i < existList.size(); ++i ) 
    {   
        if ( !newList.contains( existList.at( i ) ) )
        {
            Database::deleteSrFromDB( existList.at( i ), mDB );
        }
    }
    
    QSqlDatabase::database( mDB ).commit();
    
    updateQueueBrowser();
}

void Data::updateQueueBrowser()
{
    QString html;
    int age = 0;
    QList<QueueSR> srlist = Database::getSrList( Settings::sortAge(), Settings::sortAsc(), mDB );
        
    html += HTML::styleSheet();
    html += HTML::pageHeader( Settings::engineer(), srlist.size() );

    for ( int i = 0; i < srlist.size(); ++i )
    {
        if ( !Settings::showAwaitingCustomer() && 
              srlist.at( i ).status == "Awaiting Customer" )
        {
                //qDebug() << "[QUEUEBROWSER] Skipping" << sr->status() << sr->id();
        }
        else if ( !Settings::showAwaitingSupport() && 
                   srlist.at( i ).status == "Awaiting Novell Support" )
        {
                //qDebug() << "[QUEUEBROWSER] Skipping" << sr->status() << sr->id();
        }
        else if ( !Settings::showStatusOthers() && 
                   srlist.at( i ).status != "Awaiting Customer" && 
                   srlist.at( i ).status != "Awaiting Novell Support" )
        {
                //qDebug() << "[QUEUEBROWSER] Skipping" << sr->status() << sr->id();
        }
        else
        {
            html += HTML::SRTable( srlist.at( i ) );
        }
    }

    emit queueDataChanged( html );
    int avgAge = age / srlist.size();
        
    qDebug() << "queuefinished" << avgAge;
}

void Data::updateQmonBrowser()
{

}

void Data::updateStatsBrowser()
{

}


void Data::qmonUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
    
    qDebug() << "qmonfinished";
}

void Data::statsUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
    
    qDebug() << "statsfinished";
}


#include "data.moc"


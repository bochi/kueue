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
#include "ui/html.h"

#include <QtXml>
#include <QDesktopServices>
#include <QHostInfo>

Data::Data()
{
    qDebug() << "[DATA] Constructing, threadID" << thread()->currentThreadId();
    
    mNAM = new QNetworkAccessManager( this );
 
    QHostInfo info = QHostInfo::fromName( Settings::dBServer() );
    QList<QHostAddress> al = info.addresses();
    
    for ( int i = 0; i < al.size(); ++i ) 
    { 
        mIPs.append( al.at( i ).toString() );
    }
    
    mDB = "db-" + QString::number( thread()->currentThreadId() );
    
    QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
    
    Database::openDbConnection( mDB );
    
    QTimer* queueTimer = new QTimer( this );
    queueTimer->start( 84726 );
    
    connect( queueTimer, SIGNAL( timeout() ),
             this, SLOT( updateQueue() ) );
    
    updateQueue();

    if ( Settings::monitorEnabled() )
    {
        Database::dropQmon();
        
        QTimer* qmonTimer = new QTimer( this );
        qmonTimer->start( 51219 );
    
        connect( qmonTimer, SIGNAL( timeout() ),
                 this, SLOT( updateQmon() ) );
        
        updateQmon();
    }
    
    if ( Settings::statsEnabled() )
    {
        QTimer* statsTimer = new QTimer( this );
        statsTimer->start( 905180 );
    
        connect( statsTimer, SIGNAL( timeout() ),
                 this, SLOT( updateStats() ) );
        
        updateStats();
    }
}

Data::~Data()
{
    qDebug() << "[DATA] Destroying";
}

QNetworkReply* Data::get( const QString& u )
{
    int r = qrand() % mIPs.size();
    QNetworkRequest request( QUrl( "http://" + mIPs.at(r) + ":8080/" + u ) );
    
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
    QNetworkReply* r = get( "userqueue/" + Settings::engineer() );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( queueUpdateFinished() ) );
}

void Data::updateQmon()
{
    QNetworkReply* r = get( "qmon" );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( qmonUpdateFinished() ) );
}

void Data::updateStats()
{
    QNetworkReply* r = get( "stats/" + Settings::engineer() );
    
    connect( r, SIGNAL( finished() ), 
             this, SLOT( statsUpdateFinished() ) );
}

void Data::queueUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
    
    QDomDocument doc;
    doc.setContent( r->readAll() );
    QDomNodeList list = doc.elementsByTagName( "sr" );
    
    PersonalQueue q;
    
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
        
        q.srList.append( sr );
    }
    
    Database::updateQueue( q, mDB );   
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

    int avgAge = age / srlist.size();
    
    if ( !html.isEmpty() )
    {
        emit queueDataChanged( html );
    }
}

void Data::updateQmonBrowser()
{
    QString html;
    int age = 0;
    
    QStringList list = Settings::queuesToMonitor();
    
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
        
        QList< QmonSR > l( Database::getQmonQueue( list.at( i ).split( "|" ).at( 1 ), mDB ) );
    
        for ( int i = 0; i < l.size(); ++i ) 
        {
            html += HTML::qmonSrInQueue( l.at( i ) );
        }
        
        html += HTML::qmonTableFooter();
    }

    if ( !html.isEmpty() )
    {
        emit qmonDataChanged( html );
    }
}

void Data::updateStatsBrowser()
{
    Statz s = Database::getStatz( mDB );
    
    QString html = HTML::statsPageHeader( Database::getStatz( mDB ) );
    
    if ( !html.isEmpty() )
    {
        emit statsDataChanged( html );
    }
}

void Data::qmonUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
    
    QmonData q;
    
    QDomDocument doc;
    doc.setContent( r->readAll() );
    QDomNodeList list = doc.elementsByTagName( "sr" );
    
    for ( int i = 0; i < list.size(); ++i ) 
    {   
        QmonSR sr;
        
        sr.id = list.at( i ).namedItem( "id" ).toElement().text(); 
        sr.queue = list.at( i ).namedItem( "queue" ).toElement().text(); 
        sr.bomgarQ = list.at( i ).namedItem( "bomgarQ" ).toElement().text(); 
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
        sr.source = list.at( i ).namedItem( "source" ).toElement().text(); 
        sr.support_program = list.at( i ).namedItem( "support_program" ).toElement().text(); 
        sr.support_program_long = list.at( i ).namedItem( "support_program_long" ).toElement().text(); 
        sr.routing_product = list.at( i ).namedItem( "routing_product" ).toElement().text(); 
        sr.support_group_routing = list.at( i ).namedItem( "support_group_routing" ).toElement().text(); 
        sr.int_type = list.at( i ).namedItem( "int_type" ).toElement().text(); 
        sr.subtype = list.at( i ).namedItem( "subtype" ).toElement().text(); 
        sr.service_level = list.at( i ).namedItem( "service_level" ).toElement().text().toInt(); 
        sr.category = list.at( i ).namedItem( "category" ).toElement().text(); 
        sr.respond_via = list.at( i ).namedItem( "respond_via" ).toElement().text(); 
        sr.agesec = list.at( i ).namedItem( "age" ).toElement().text().toInt(); 
        sr.lastupdatesec = list.at( i ).namedItem( "lastupdate" ).toElement().text().toInt(); 
        sr.timeinqsec = list.at( i ).namedItem( "timeinQ" ).toElement().text().toInt(); 
        sr.slasec = list.at( i ).namedItem( "sla" ).toElement().text().toInt(); 
        sr.highvalue = list.at( i ).namedItem( "highvalue" ).toElement().text().toInt();
        sr.critsit = list.at( i ).namedItem( "critsit" ).toElement().text().toInt();
        
        q.srList.append( sr );
    }
    
    q.total = q.srList.size();
    
    Database::updateQmon( q, mDB );
    
    updateQmonBrowser();
}

void Data::statsUpdateFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>( sender() );
 
    Statz statz;
    
    QDomDocument doc;
    doc.setContent( r->readAll() );
    
    QDomElement root = doc.documentElement();
    
    QList<Survey> surveyItemList;
    QList<ClosedItem> closedItemList;
    
    QDomNode n = root.firstChild();
    
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
  
        if( !e.isNull() )
        {     
            if( e.tagName() == "closed" )
            {
                QDomNodeList cList = e.childNodes();

                for ( int i = 0; i < cList.size(); ++i ) 
                {   
                    ClosedItem ci;
                    
                    ci.id = cList.at( i ).namedItem( "sr" ).toElement().text();
                    ci.customer =  cList.at( i ).namedItem( "customer" ).toElement().text();
                    ci.bdesc = cList.at( i ).namedItem( "bdesc" ).toElement().text();
                    ci.tts = cList.at( i ).namedItem( "tts" ).toElement().text().toInt();
                    
                    statz.closedList.append( ci );
                }
            }
            else if ( e.tagName() == "csat" )
            {
                QDomNodeList csatList = e.childNodes();

                for ( int i = 0; i < csatList.size(); ++i ) 
                {   
                    Survey s;
            
                    s.id = csatList.at( i ).namedItem( "sr" ).toElement().text();
                    s.customer = csatList.at( i ).namedItem( "customer" ).toElement().text();
                    s.bdesc = csatList.at( i ).namedItem( "bdesc" ).toElement().text();
                    s.rts = csatList.at( i ).namedItem( "rts" ).toElement().text().toInt();
                    s.engsat = csatList.at( i ).namedItem( "engsat" ).toElement().text().toInt();
                    s.srsat = csatList.at( i ).namedItem( "srsat" ).toElement().text().toInt();
                                
                    statz.surveyList.append( s );
                }
            }
            else if ( e.tagName() == "closeddata" )
            {
                QDomNode closednode = e.firstChild();
                
                while ( !closednode.isNull() )
                {
                    QDomElement closedelement = closednode.toElement();
                    
                    if ( !closedelement.isNull() )
                    {
                        if ( closedelement.tagName() == "srs" )
                        {
                            statz.closedSr = closedelement.text().toInt();
                        }
                        else if ( closedelement.tagName() == "crs" )
                        {
                            statz.closedCr = closedelement.text().toInt();
                        }
                        else if ( closedelement.tagName() == "srttsavg" )
                        {
                            statz.srTtsAvg = closedelement.text().toInt();
                        }
                    }
                    
                    closednode = closednode.nextSibling();
                }
            }
            else if ( e.tagName() == "csatdata" )
            {
                QDomNode csatnode = e.firstChild();
                
                while ( !csatnode.isNull() )
                {
                    QDomElement csatelement = csatnode.toElement();
                    
                    if ( !csatelement.isNull() )
                    {
                        if ( csatelement.tagName() == "engavg" )
                        {
                            statz.csatEngAvg = csatelement.text().toInt();
                        }
                        else if ( csatelement.tagName() == "sravg" )
                        {
                            statz.csatSrAvg = csatelement.text().toInt();
                        }
                        else if ( csatelement.tagName() == "rtsavg" )
                        {
                            statz.csatRtsPercent = csatelement.text().toInt();
                        }
                    }
                    
                    csatnode = csatnode.nextSibling();
                }
            }
        }
        
        n = n.nextSibling();
    }
    
    Database::updateStats( statz, mDB );
    updateStatsBrowser();
}

#include "data.moc"


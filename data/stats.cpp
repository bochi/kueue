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

#include "stats.h"
#include "data/database.h"
#include "kueue.h"
#include "ui/html.h"

#include <QDebug>
#include <QMenu>
#include <QWidgetAction>
#include <QToolButton>
#include <QWebElementCollection>
#include <QWebFrame>

Stats::Stats()
{
    qDebug() << "[STATS] Constructing";
    
    mTimer = new QTimer( this );

    connect( mTimer, SIGNAL( timeout() ), 
             this, SLOT( update() ) );
        
    if ( Settings::statsEnabled() )
    {
        mTimer->start( Settings::statsMinutes() * 60000 );
        update();
    }
}

Stats::~Stats()
{
    qDebug() << "[STATS] Destroying";
    
    if ( mTimer != 0 )
    {
        mTimer->stop();
        delete mTimer;
        mTimer = 0;
    }
}

void Stats::update()
{
    updatePersonal();
}

void Stats::updatePersonal()
{
    mCsat = Kueue::download( QUrl( Settings::dBServer() + "/custsat.asp?wf=" + getWF( Settings::engineer() ) ) );
    
    connect( mCsat, SIGNAL( finished() ), 
             this, SLOT( csatJobDone() ) );
    
    mTts = Kueue::download( QUrl( Settings::dBServer() + "/timetosolutiontse.asp?tse=" + Settings::engineer() ) );
    
    connect( mTts, SIGNAL( finished() ), 
             this, SLOT( ttsJobDone() ) );
}

void Stats::updateTeam()
{
    // update team stats
}

void Stats::csatJobDone()
{
    QString csat = mCsat->readAll();
    QStringList existList( Database::getCsatExistList() );
    QStringList newList;
    QStringList data;
    QString customer;
    QString bdesc;
    bool changed = false;
    
    if ( !mCsat->error() && 
       ( csat.contains( "<br>" ) ) )
    {
        csat.remove( "<HTML>" );
        data = csat.split( "<br>" );

	for ( int i = 0; i < data.size(); ++i ) 
        {
            if ( !data.at( i ).isEmpty() )
            { 
                newList.append( Database::getEngineerForWfid( data.at( i ).split( "|" ).at( 0 ).trimmed() ) + 
                                "|" + data.at( i ).split( "|" ).at( 1 ) );
            }
            
            if ( !data.at( i ).isEmpty() && 
                 !Database::csatExistsInDB( data.at( i ).split( "|" ).at( 1 ), Database::getEngineerForWfid( data.at( i ).split( "|" ).at( 0 ).trimmed() ) ) )
            {
                QString srdata;
                QNetworkReply *reply = Kueue::download( QUrl( Settings::dBServer() + "/stefan.asp?sr=" + data.at( i ).split( "|" ).at( 1 ) ) );
 
                QEventLoop loop;
                
                QObject::connect( reply, SIGNAL( finished() ), 
                                  &loop, SLOT( quit() ) );
                
                loop.exec();
 
                srdata = reply->readAll();
                srdata.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );

                if ( srdata.contains( "|||" ) )
                {
                    customer = srdata.split("|||").at( 4 ).trimmed() + " (" + srdata.split("|||").at( 5 ).trimmed() + ")";
                    bdesc = srdata.split("|||").at( 6 ).trimmed();
                }
            
                CsatItem* ci = new CsatItem;
                    
                ci->engineer = Database::getEngineerForWfid( data.at( i ).split( "|" ).at( 0 ).trimmed() );
                ci->sr = data.at( i ).split( "|" ).at( 1 );
                ci->customer = customer;
                ci->bdesc = bdesc;
                ci->srsat = data.at( i ).split( "|" ).at( 2 );
                ci->engsat = data.at( i ).split( "|" ).at( 3 );
                
                if ( data.at( i ).split( "|" ).at( 4 ) == "1" )
                {
                    ci->rts = true;
                }
                else
                {
                    ci->rts = false;
                }
                
                Database::updateCsatData( ci );
                changed = true;
                
                delete ci;
                reply->deleteLater();
            }
        }
    
        for ( int i = 0; i < existList.size(); ++i )
        {
            if ( !newList.contains( existList.at( i ) ) ) 
            {
                Database::deleteCsatItemFromDB( existList.at( i ).split( "|" ).at( 1 ), existList.at( i ).split( "|" ).at( 0 ) );
                changed = true;
            }
        }
    }       
    
    if ( changed )
    {
        emit statsChanged();
    }
}

void Stats::ttsJobDone()
{
    QString tts = mTts->readAll();
    QStringList data;
    QStringList existList( Database::getTtsExistList() );
    QStringList newList;
    bool changed = false;
    bool initial = false;
    
    if ( existList.isEmpty() )
    {
        initial = true;
    }
    
    if ( !mTts->error() && ( tts.contains( "<br>" ) ) )
    {
        tts.remove( "<HTML>" );
        data = tts.split( "<br>" );
        
        if ( initial )
        {
            //emit initialUpdate( data.size(), 3 );
        }
        
        for ( int i = 0; i < data.size(); ++i ) 
        {
            if ( !data.at( i ).isEmpty() )
            {
                newList.append( data.at( i ).split( "|" ).at( 1 ) );
            }
        
            if ( !data.at( i ).isEmpty() && !Database::ttsExistsInDB( data.at( i ).split( "|" ).at( 1 ) ) )
            {
                TtsItem* ti = new TtsItem;
                QString srdata;
                
                ti->engineer = data.at( i ).split( "|" ).at( 0 ).trimmed();
                ti->sr = data.at( i ).split( "|" ).at( 1 ).trimmed();
                ti->tts = data.at( i ).split( "|" ).at( 2 ).trimmed().toInt();
            
                QNetworkReply *reply = Kueue::download( QUrl( Settings::dBServer() + "/stefan.asp?sr=" + ti->sr ) );
                QEventLoop loop;
 
                QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
                loop.exec();
                         
                srdata = reply->readAll();
                srdata.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );

                if ( srdata.contains( "|||" ) )
                {
                    ti->customer = srdata.split("|||").at( 4 ).trimmed() + " (" + srdata.split("|||").at( 5 ).trimmed() + ")";
                    ti->bdesc = srdata.split("|||").at( 6 ).trimmed();
                }
                
                if ( initial )
                {
                    emit initialUpdateProgress( i );
                }
                
                Database::updateTtsData( ti );
                changed = true;
            }
        }
        
        if ( initial )
        {
            //emit initialUpdateDone();
        }
        
        for ( int i = 0; i < existList.size(); ++i )
        {
            if ( !newList.contains( existList.at( i ) ) ) 
            {
                Database::deleteTtsItemFromDB( existList.at( i ) );
                changed = true;
            }
        }
    }
   
    if ( changed )
    {
        emit statsChanged();
    }
}

QString Stats::getWF( const QString& engineer )
{
    if ( Database::getWfid( engineer ) == "NOTFOUND" )
    {
        QEventLoop loop;
        QString wfid;
    
        QNetworkReply *reply = Kueue::download( QUrl( Settings::dBServer() + "/workforce.asp?tse=" + engineer ) );
    
        loop.connect( reply, SIGNAL( readyRead() ),
                      &loop, SLOT( quit() ) );
        
        loop.exec();
       
        wfid = reply->readAll();
        wfid.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
    
        Database::updateWFID( engineer, wfid.trimmed() );
    
        return wfid.trimmed();
    }
    else 
    {
        return Database::getWfid( engineer );
    }
}

#include "stats.moc"

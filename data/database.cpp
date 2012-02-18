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

#include "database.h"
#include "ui/html.h"

#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QDesktopServices>

Database::Database()
{   
    qDebug() << "[DATABASE] Constructing";
    
    QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );

    if ( !dir.exists() )
    {
        dir.mkpath( dir.path() );
    }
    
    mDBfile = dir.path() + "/database.sqlite";
    
    mDb = QSqlDatabase::addDatabase( "QSQLITE" );
    mDb.setDatabaseName( mDBfile );
    
    if ( !mDb.open() )
    {
        qDebug() << "[DATABASE] Failed to open the database.";
    }
                         
    QSqlQuery query( mDb );
    
    if ( !query.exec("PRAGMA temp_store = MEMORY") )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec("PRAGMA synchronous = OFF") )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec("PRAGMA journal_mode = MEMORY") )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec("PRAGMA locking_mode = EXCLUSIVE") )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec( "CREATE TABLE IF NOT EXISTS " + Settings::engineer() +
                      "( ID INTEGER PRIMARY KEY UNIQUE, CDATE TEXT, ADATE TEXT, STATUS TEXT, CUSTOMER TEXT, CONTACT TEXT, "
                      "BDESC TEXT, DDESC TEXT, SS INTEGER, DISPLAY TEXT )" ) )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }

    if ( !query.exec( "CREATE TABLE IF NOT EXISTS qmon_siebel( ID INTEGER PRIMARY KEY UNIQUE, QUEUE TEXT, SEVERITY TEXT, HOURS TEXT, "
                      "SOURCE TEXT, CONTACTVIA TEXT, ODATE TEXT, ADATE TEXT, STATUS TEXT, "
                      "CONTRACT TEXT, QUEUE1 TEXT, PHONE TEXT, ONSITEPHONE TEXT, GEO TEXT, "
                      "WTF TEXT, ROUTING TEXT, BDESC TEXT, SLA TEXT, DISPLAY TEXT, TIQ TEXT, BOMQ TEXT )" ) )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec( "CREATE TABLE IF NOT EXISTS qmon_chat( ID TEXT PRIMARY KEY UNIQUE, SR INTEGER, REPTEAM TEXT, "
                      "NAME TEXT, DATE TEXT, SOMENR INTEGER )" ) )
        {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec( "CREATE TABLE IF NOT EXISTS workforce_ids( ENGINEER TEXT, WFID INTEGER PRIMARY KEY UNIQUE )" ) )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
    
    if ( !query.exec( "CREATE TABLE IF NOT EXISTS csat ( ENGINEER TEXT, SR INTEGER PRIMARY KEY UNIQUE, CUSTOMER TEXT, "
                      "BDESC TEXT, SRSAT INTEGER, ENGSAT INTEGER, RTS INTEGER )" ) )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
        
    if ( !query.exec( "CREATE TABLE IF NOT EXISTS tts ( ENGINEER TEXT, SR INTEGER PRIMARY KEY UNIQUE, TTS INTEGER, BDESC TEXT, CUSTOMER TEXT )" ) )
    {
        qDebug() << "[DATABASE] Error:" << query.lastError();
    }
}

Database::~Database()
{   
    qDebug() << "[DATABASE] Destroying";

    mDb.close();    
    QSqlDatabase::removeDatabase( mDb.connectionName() );
}

/*


                P E R S O N A L   Q U E U E


*/


void Database::getSRData( SR* sr )
{
    QSqlQuery query;
    query.prepare( "SELECT ID, CDATE, ADATE, STATUS, CUSTOMER, CONTACT, BDESC, DDESC, SS, DISPLAY "
                   "FROM " + Settings::engineer() + " WHERE ID = :id" );
    
    query.bindValue( ":id", sr->id() );
    query.exec();

    if ( query.next() )
    {
        sr->setOpened( QDateTime::fromString( query.value( 1 ).toString(), "yyyy-MM-dd hh:mm:ss" ) );
        sr->setLastUpdate( QDateTime::fromString( query.value( 2 ).toString(), "yyyy-MM-dd hh:mm:ss" ) );
        sr->setStatus( query.value( 3 ).toString() );
        sr->setCustomer( query.value( 4 ).toString() );
        sr->setContact( query.value( 5 ).toString() );
        sr->setBriefDesc( query.value( 6 ).toString() );
        sr->setDetailedDesc( query.value( 7 ).toString() );

        if ( query.value( 8 ) == "true" ) 
        {
            sr->setSs( true );
        }
        else 
        {
            sr->setSs( false );
        }

        if ( sr->customer().contains( "COLLABORATION" ) )
        {    
            sr->setIsCR( true );
        }
        else 
        {    
            sr->setIsCR( false );
        }

        sr->setDisplay( query.value( 9 ).toString() );
        
        QDateTime now = QDateTime::currentDateTime();
   
        sr->setAge( sr->opened().daysTo( now ) );
        sr->setLastUpdateDays( sr->lastUpdate().daysTo( now ) );

    }
    else qDebug() << "[DATABASE] Failed to get SR data for" << sr->id() << query.executedQuery() << query.lastError();
}

void Database::updateSRData( SR* sr )
{    
    QSqlQuery query;
    
    qDebug() << "[DATABASE] Updating SR data for" << sr->id();
    
    query.prepare(  "UPDATE " + Settings::engineer() + " SET "
                    "CDATE=:cdate, ADATE=:adate, STATUS=:status, CUSTOMER=:customer, CONTACT=:contact, "
                    "BDESC=:bdesc, DDESC=:ddesc, SS=:ss WHERE id = :id" );

    query.bindValue( ":id", sr->id() );    
    query.bindValue( ":cdate", sr->opened().toString( "yyyy-MM-dd hh:mm:ss" ) );
    query.bindValue( ":adate", sr->lastUpdate().toString( "yyyy-MM-dd hh:mm:ss" ) );
    query.bindValue( ":status", sr->status() );
    query.bindValue( ":customer", sr->customer() );
    query.bindValue( ":contact", sr->contact() );
    query.bindValue( ":bdesc", sr->briefDesc() );
    query.bindValue( ":ddesc", sr->detailedDesc() );
    query.bindValue( ":ss", sr->ss() );
    
    if ( !query.exec() ) 
    {
        qDebug() << "[DATABASE] Failed to update" << sr->id() << "in DB" << query.executedQuery() << query.lastError();
    }
}

void Database::insertSRData( SR* sr )
{
    qDebug() << "[DATABASE] Inserting SR data for" << sr->id();
    
    QSqlQuery query;
    
    query.prepare(  "INSERT into " + Settings::engineer() +
                    "( ID, CDATE, ADATE, STATUS, CUSTOMER, CONTACT, BDESC, DDESC, SS, DISPLAY )"
                    "VALUES" 
                    "( :id, :cdate, :adate, :status, :customer, :contact, :bdesc, :ddesc, :ss, 'none' );" );
        
    query.bindValue( ":id", sr->id() );    
    query.bindValue( ":cdate", sr->opened().toString( "yyyy-MM-dd hh:mm:ss" ) );
    query.bindValue( ":adate", sr->lastUpdate().toString( "yyyy-MM-dd hh:mm:ss" ) );
    query.bindValue( ":status", sr->status() );
    query.bindValue( ":customer", sr->customer() );
    query.bindValue( ":contact", sr->contact() );
    query.bindValue( ":bdesc", sr->briefDesc() );
    query.bindValue( ":ddesc", sr->detailedDesc() );
    query.bindValue( ":ss", sr->ss() );
    
    if ( !query.exec() ) 
    {
        qDebug() << "[DATABASE] Failed to add" << sr->id() << "into DB" << query.executedQuery() << query.lastError();
    }
}

void Database::deleteSrFromDB( const QString& id )
{
    qDebug() << "[DATABASE] Deleting SR" << id;
    
    QSqlQuery query;
    query.prepare( "DELETE FROM " + Settings::engineer() +" WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
}

bool Database::srExistsInDB( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT ID FROM " + Settings::engineer() +" WHERE ( ID = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return true;
    }
    else 
    {    
        return false;
    }
}

QString Database::getAdate( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT ADATE FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}

QString Database::getStatus( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT STATUS FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}

void Database::setDisplay( const QString& d )
{
    QSqlQuery query;
    
    query.prepare(  "UPDATE " + Settings::engineer() + " SET "
                    "DISPLAY=:display "
                    "WHERE id = :id" );
    
    query.bindValue( ":display", d.split( "-" ).at( 1 ) );
    query.bindValue( ":id", d.split( "-" ).at( 0 ) );
    query.exec();
}

void Database::closeAllTables()
{
    QSqlQuery query;
    
    query.prepare(  "UPDATE " + Settings::engineer() + " SET "
                    "DISPLAY='none' " );
        
    query.exec();
}

void Database::expandAllTables()
{
    QSqlQuery query;
    
    query.prepare(  "UPDATE " + Settings::engineer() + " SET "
                    "DISPLAY='block' " );
    
    query.exec();
}

QStringList Database::getSrList( bool s, bool a )
{
    QStringList l;
    QSqlQuery query;
    
    if ( a )
    {
        if ( s ) 
        {    
            query.prepare( "SELECT ID FROM " + Settings::engineer() +" ORDER BY CDATE ASC" );
        }
        else 
        {    
            query.prepare( "SELECT ID FROM " + Settings::engineer() +" ORDER BY ADATE ASC" );
        }
    }
    else
    {
        if ( s ) 
        {    
            query.prepare( "SELECT ID FROM " + Settings::engineer() +" ORDER BY CDATE DESC" );
        }
        else 
        {    
            query.prepare( "SELECT ID FROM " + Settings::engineer() +" ORDER BY ADATE DESC" );
        }
    }
        
    query.exec();
    
    while( query.next() )
    {
        l.append( query.value( 0 ).toString() );
    }
    
    return l;
}

int Database::srsTotal()
{
    return getSrList().count();
}

QString Database::getDetailedDescription( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT DDESC FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}

QString Database::getSrStatus( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT STATUS FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}

QString Database::getCustomer( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT CUSTOMER, CONTACT FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString() + " (" + query.value( 1 ).toString() + ")";
    }
    else
    {
        return "ERROR";
    }
}

QString Database::getBriefDescription( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT BDESC FROM " + Settings::engineer() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}

/*


                M O N I T O R


*/

void Database::insertSiebelItemIntoDB( SiebelItem* item )
{
    qDebug() << "[DATABASE] Inserting SiebelItem for " << item->id << item->queue;
      
    QSqlQuery query( "INSERT INTO qmon_siebel( ID, QUEUE, SEVERITY, HOURS, SOURCE, CONTACTVIA, ODATE, ADATE, "
                     "STATUS, CONTRACT, QUEUE1, PHONE, ONSITEPHONE, GEO, WTF, ROUTING, BDESC, SLA, TIQ, BOMQ )"
                     "VALUES"
                     "( :id, :queue, :severity, :hours, :source, :contactvia, :odate, :adate, :status, :contract, "
                     ":queue1, :phone, :onsitephone, :geo, :wtf, :routing, :bdesc, :sla, :tiq, :bomq )" );
  
    query.bindValue( ":id", item->id );
    query.bindValue( ":queue", item->queue );
    query.bindValue( ":severity", item->severity );
    query.bindValue( ":hours", item->hours );
    query.bindValue( ":source", item->source );
    query.bindValue( ":contactvia", item->contactvia );
    query.bindValue( ":odate", item->openedSec );
    query.bindValue( ":adate", item->activitySec );
    query.bindValue( ":status", item->status );
    query.bindValue( ":contract", item->contract );
    query.bindValue( ":queue1", item->queue1 );
    query.bindValue( ":phone", item->phone );
    query.bindValue( ":onsitephone", item->onsitephone );
    query.bindValue( ":geo", item->geo );
    query.bindValue( ":wtf", item->wtf );
    query.bindValue( ":routing", item->routing );
    query.bindValue( ":bdesc", item->bdesc );
    query.bindValue( ":sla", item->slaSec );
    query.bindValue( ":tiq", item->queueSec );
    query.bindValue( ":bomq", item->bomgarQ );

    query.exec();
}

void Database::updateSiebelItemInDB( SiebelItem* item )
{
    QSqlQuery query;
    
    query.prepare(  "UPDATE qmon_siebel SET "
                    "QUEUE=:queue, SEVERITY=:severity, ODATE=:odate, ADATE=:adate, STATUS=:status, "
                    "SLA=:sla, TIQ=:tiq, BOMQ=:bomq WHERE id = :id" );

    query.bindValue( ":queue", item->queue );
    query.bindValue( ":severity", item->severity );
    query.bindValue( ":odate", item->openedSec );
    query.bindValue( ":adate", item->activitySec );
    query.bindValue( ":status", item->status );
    query.bindValue( ":sla", item->slaSec );
    query.bindValue( ":tiq", item->queueSec );
    query.bindValue( ":bomq", item->bomgarQ );
    query.bindValue( ":id", item->id );    
    
    query.exec();
}


void Database::updateSiebelQueue( SiebelItem* si )
{
    qDebug() << "[DATABASE] Updating Siebel Queue" << si->id << si->queue;
    QSqlQuery query( "UPDATE qmon_siebel SET QUEUE = :queue WHERE id = :id" );
                
    query.bindValue( ":queue", si->queue );
    query.bindValue( ":id", si->id );
                
    query.exec();
}

void Database::updateSiebelSeverity( SiebelItem* si )
{
    qDebug() << "[DATABASE] Updating Siebel Severity" << si->id << si->severity;
    QSqlQuery query( "UPDATE qmon_siebel SET SEVERITY = :severity WHERE id = :id" );
                
    query.bindValue( ":severity", si->severity );
    query.bindValue( ":id", si->id );
                
    query.exec();
}

void Database::updateSiebelDisplay( const QString& display )
{
    QSqlQuery query( "UPDATE qmon_siebel SET DISPLAY = :display WHERE id = :id" );
                
    query.bindValue( ":display", display.split( "-" ).at( 1 ) );
    query.bindValue( ":id", display.split( "-" ).at( 0 ) );
                
    query.exec();   
}

void Database::deleteSiebelItemFromDB( const QString& id )
{
    qDebug() << "[DATABASE] Deleting SiebelItem" << id;
    
    QSqlQuery query;
    query.prepare( "DELETE FROM qmon_siebel WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
}

QStringList Database::getQmonSiebelList()
{
    QSqlQuery query;
    QStringList l;
    query.prepare( "SELECT ID FROM qmon_siebel" );
    query.exec();
    
    while( query.next() )
    {
        l.append( query.value( 0 ).toString() );
    }
    
    return l;    
}

bool Database::siebelExistsInDB( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT ID FROM qmon_siebel WHERE ( ID = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return true;
    }
    else 
    {    
        return false;
    }
}

bool Database::siebelQueueChanged( SiebelItem* si  )
{
    QSqlQuery query;
    query.prepare( "SELECT QUEUE FROM qmon_siebel WHERE ( ID = :id )" );
    query.bindValue( ":id", si->id );
    query.exec();

    if ( query.next() )
    {
        if ( query.value( 0 ).toString() == si->queue )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

bool Database::siebelSeverityChanged( SiebelItem* si  )
{
    QSqlQuery query;
    query.prepare( "SELECT SEVERITY FROM qmon_siebel WHERE ( ID = :id )" );
    query.bindValue( ":id", si->id );
    query.exec();

    if ( query.next() )
    {
        if ( query.value( 0 ).toString() == si->severity )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

bool Database::isChat( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT ID FROM qmon_chat WHERE ( SR = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return true;
    }
    else 
    {    
        return false;
    }
}

QString Database::getQmonBdesc( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT BDESC FROM qmon_siebel WHERE ( ID = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "ERROR";
    }
}


/*


                B O M G A R 


*/

void Database::updateBomgarItemInDB( BomgarItem* bi )
{
    qDebug() << "[DATABASE] Inserting BomgarItem" << bi->id << bi->sr;
        
    QSqlQuery query( "INSERT INTO qmon_chat( ID, SR, REPTEAM, NAME, DATE, SOMENR )"
                     "VALUES"
                     "( :id, :sr, :repteam, :name, :date, :somenr )" );
                     
    query.bindValue( ":id", bi->id );
    query.bindValue( ":sr", bi->sr );
    query.bindValue( ":repteam", bi->repteam );
    query.bindValue( ":name", bi->name );
    query.bindValue( ":date", bi->date );
    query.bindValue( ":somenr", bi->someNumber );
    query.exec();
}

void Database::deleteBomgarItemFromDB( const QString& id )
{
    qDebug() << "[DATABASE] Deleting BomgarItem" << id;
    
    QSqlQuery query;
    query.prepare( "DELETE FROM qmon_chat WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
}

QList< SiebelItem* > Database::getSrsForQueue( const QString& queue )
{
    QSqlQuery query;
    QList< SiebelItem* > list;
    
    query.prepare( "SELECT ID, SEVERITY, HOURS, SOURCE, CONTACTVIA, ODATE, ADATE, STATUS, CONTRACT, GEO, BDESC, SLA, DISPLAY, TIQ, BOMQ "
                   "FROM qmon_siebel WHERE ( QUEUE = :queue )" );
    query.bindValue( ":queue", queue );
    query.exec();
    
    while ( query.next() ) 
    {
        SiebelItem* si = new SiebelItem;
        
        si->id = query.value( 0 ).toString();
        si->severity = query.value( 1 ).toString();
        si->hours = query.value( 2 ).toString();
        si->source = query.value( 3 ).toString();
        si->contactvia = query.value( 4 ).toString();
        si->activitySec = query.value( 5 ).toInt();
        si->openedSec = query.value( 6 ).toInt();
        si->status = query.value( 7 ).toString();
        si->contract = query.value( 8 ).toString();
        si->geo = query.value( 9 ).toString();
        si->bdesc = query.value( 10 ).toString();
        si->slaSec = query.value( 11 ).toInt();
        si->display = query.value( 12 ).toString();
        si->queueSec = query.value( 13 ).toInt();
        si->bomgarQ = query.value( 14 ).toString();
        
        if ( si->bomgarQ.isEmpty() )
        {
            si->isChat = false;
        }
        else
        {
            si->isChat = true;
        }
        
        list.append( si );
    }
        
    return list;
}

QStringList Database::getQmonBomgarList()
{
    QSqlQuery query;
    QStringList l;
    query.prepare( "SELECT ID FROM qmon_chat" );
    query.exec();
    
    while( query.next() ) l.append( query.value( 0 ).toString() );
    
    return l;    
}

bool Database::bomgarExistsInDB( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT ID FROM qmon_chat WHERE ( ID = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() )
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void Database::updateBomgarQueue( BomgarItem* bi )
{
    qDebug() << "[DATABASE] Updating BomgarQueue for" << bi->id << bi->sr << " to " << bi->name;
    
    QSqlQuery query;
    query.prepare( "UPDATE qmon_chat SET NAME = :name WHERE ID = :id" );
    query.bindValue( ":name", bi->name );
    query.bindValue( ":id", bi->id );
    query.exec();
}

QString Database::getBomgarQueue( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT NAME FROM qmon_chat WHERE ( SR = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if( query.next() ) 
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "NOCHAT";
    }
}

QString Database::getBomgarQueueById( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT NAME FROM qmon_chat WHERE ( ID = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if( query.next() ) 
    {
        return query.value( 0 ).toString();
    }
    else
    {
        return "NOCHAT";
    }
}

/*

                C S A T


*/

void Database::updateCsatData( CsatItem* ci )
{
    QSqlQuery query;
 
    query.prepare( "INSERT INTO csat ( ENGINEER, SR, CUSTOMER, BDESC, SRSAT, ENGSAT, RTS )"
                   "VALUES ( :engineer, :sr, :customer, :bdesc, :srsat, :engsat, :rts )" );
    
    query.bindValue( ":engineer", ci->engineer );
    query.bindValue( ":sr", ci->sr );
    query.bindValue( ":customer", ci->customer );
    query.bindValue( ":bdesc", ci->bdesc );
    query.bindValue( ":srsat", ci->srsat );
    query.bindValue( ":engsat", ci->engsat );
    
    if ( ci->rts )
    {
        query.bindValue( ":rts", "1" );
    }
    else
    {
        query.bindValue( ":rts", "0" );
    }
    
    query.exec();
}

void Database::deleteCsatItemFromDB( const QString& sr, const QString& engineer )
{
    QSqlQuery query;

    query.prepare( "DELETE from csat WHERE ( ENGINEER = :engineer )"
                   "AND ( SR = :sr )" );
    query.bindValue( ":engineer", engineer );
    query.bindValue( ":sr", sr );
    
    query.exec();
}

bool Database::csatExistsInDB( const QString& id, const QString& engineer )
{
    QSqlQuery query;
    query.prepare( "SELECT * FROM csat WHERE ( SR = :id ) AND ( ENGINEER = :engineer )" );
    query.bindValue( ":id", id );
    query.bindValue( ":engineer", engineer );
    query.exec();
    
    if ( query.next() ) 
    {
        return true;
    }
    else 
    {    
        return false;
    }
}

QList< CsatItem* > Database::getCsatList( const QString& engineer )
{
    QSqlQuery query;
    QList< CsatItem* > list;
    
    if ( engineer == "NONE" )
    {
        query.prepare( "SELECT * from csat" );
    }
    else
    {
        query.prepare( "SELECT * FROM csat WHERE ( ENGINEER = :engineer )" );
        query.bindValue( ":engineer", engineer );
    }
    
    query.exec();
    
    while( query.next() )
    {
        CsatItem* ci = new CsatItem;
        
        ci->engineer = query.value( 0 ).toString();
        ci->wfid = getWfid( query.value( 0 ).toString() );
        ci->sr = query.value( 1 ).toString();
        ci->customer = query.value( 2 ).toString();
        ci->bdesc = query.value( 3 ).toString();
        ci->srsat = query.value( 4 ).toString();
        ci->engsat = query.value( 5 ).toString();
        
        if ( query.value( 6 ).toString() == "1" )
        {
            ci->rts = true;
        }
        else
        {
            ci->rts = false;
        }
        
        list.append( ci );
    }
    
    return list;
}

QStringList Database::getCsatExistList()
{
    QSqlQuery query;
    QStringList list;
    
    query.prepare( "SELECT ENGINEER, SR FROM csat" );
    query.exec();
    
    while( query.next() )
    {
        list.append( query.value( 0 ).toString() + "|" + query.value( 1 ).toString() );
    }
    
    return list;
}

int Database::csatEngAverage( const QString& engineer )
{
    QSqlQuery query;
    long long avg = 0;
    int cnt = 0;
    
    query.prepare( "SELECT ENGSAT FROM csat WHERE ( ENGINEER = :engineer )" );
    
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer() );
    }
    else
    {
        query.bindValue( ":engineer", engineer );
    }
    
    query.exec();
    
    while( query.next() )
    {
        cnt++;
        avg = avg + query.value( 0 ).toLongLong();
    }
   
    if ( cnt > 0 )
    {
        return ( avg / cnt );
    }
    else
    {
        return 0;
    }
}

int Database::csatSrAverage( const QString& engineer )
{
    QSqlQuery query;
    long long avg = 0;
    int cnt = 0;
    
    query.prepare( "SELECT SRSAT FROM csat WHERE ( ENGINEER = :engineer )" );
    
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer() );
    }
    else
    {
        query.bindValue( ":engineer", engineer );
    }
    
    query.exec();
    
    while( query.next() )
    {
        cnt++;
        avg = avg + query.value( 0 ).toLongLong();
    }
   
    if ( cnt > 0 )
    {
        return ( avg / cnt );
    }
    else
    {
        return 0;
    }
}

int Database::csatRtsPercent( const QString& engineer )
{
    QSqlQuery query;
    long long avg = 0;
    int cnt = 0;
    
    query.prepare( "SELECT RTS FROM csat WHERE ( ENGINEER = :engineer )" );
    
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer() );
    }
    else
    {
        query.bindValue( ":engineer", engineer );
    }
    
    query.exec();
    
    while( query.next() )
    {
        cnt++;
        avg = avg + query.value( 0 ).toLongLong();
    }
   
    if ( cnt > 0 )
    {
        return ( avg * 100 / cnt );
    }
    else
    {
        return 0;
    }
}

/*


                W F I D


*/

void Database::updateWFID( const QString& engineer, const QString& wfid )
{
    QSqlQuery query;
    
    if ( getWfid( engineer ) == "NOTFOUND" )
    {
        query.prepare( "INSERT INTO workforce_ids ( ENGINEER, WFID ) VALUES ( :engineer, :wfid )" );
    }
    else
    {
        query.prepare( "UPDATE workforce_ids SET WFID=:wfid WHERE ENGINEER=:engineer" );
    }
 
    query.bindValue( ":engineer", engineer );
    query.bindValue( ":wfid", wfid );
        
    if ( !query.exec() )
    {
        qDebug() << "[DATABASE] Query failed" << query.executedQuery() << query.lastError();
    }            
}

QString Database::getWfid( const QString& engineer )
{
    QSqlQuery query;
    
    query.prepare( "SELECT WFID FROM workforce_ids WHERE ENGINEER = :engineer" );
    query.bindValue( ":engineer", engineer );
    query.exec();
    
    if ( !query.next() )
    {
        return QString( "NOTFOUND" );
    }
    else 
    {    
        return query.value( 0 ).toString();
    }
}

QString Database::getEngineerForWfid( const QString& wfid )
{
    QSqlQuery query;
    
    query.prepare( "SELECT ENGINEER FROM workforce_ids where WFID = :wfid" );
    query.bindValue( ":wfid", wfid );
    query.exec();
    
    if ( !query.next() )
    {
        return QString( "NOTFOUND" );
    }
    else 
    {    
        return query.value( 0 ).toString();
    }
}

/*


                T I M E  T O  S O L U T I O N


*/

void Database::updateTtsData( TtsItem* ti )
{
    QSqlQuery query;

    query.prepare( "INSERT INTO tts ( ENGINEER, SR, TTS, BDESC, CUSTOMER )"
                   "VALUES ( :engineer, :sr, :tts, :bdesc, :customer )" );
    
    query.bindValue( ":engineer", ti->engineer );
    query.bindValue( ":sr", ti->sr );
    query.bindValue( ":tts", ti->tts );
    query.bindValue( ":bdesc", ti->bdesc );
    query.bindValue( ":customer", ti->customer );
                
    query.exec();
}

QList< TtsItem* > Database::getTtsList( const QString& engineer )
{
    QSqlQuery query;
    QList< TtsItem* > list;
    
    query.prepare( "SELECT * FROM tts WHERE ( ENGINEER = :engineer )" );
    
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer().toUpper() );
    }
    else
    {
        query.bindValue( ":engineer", engineer.toUpper() );
    }
    
    query.exec();
    
    while( query.next() )
    {
        TtsItem* ti = new TtsItem;
        
        ti->engineer = query.value( 0 ).toString();
        ti->sr = query.value( 1 ).toString();
        ti->tts = query.value( 2 ).toInt();
        ti->bdesc = query.value( 3 ).toString();
        ti->customer = query.value( 4 ).toString();
        
        list.append( ti );
    }
    
    return list;
}

void Database::deleteTtsItemFromDB( const QString& id )
{
    QSqlQuery query;

    query.prepare( "DELETE from tts WHERE ( SR = :sr )" );
    query.bindValue( ":sr", id );
    
    query.exec();
}

bool Database::ttsExistsInDB( const QString& id )
{
    QSqlQuery query;
    query.prepare( "SELECT * FROM tts WHERE ( SR = :id )" );
    query.bindValue( ":id", id );
    query.exec();
    
    if ( query.next() ) 
    {
        return true;
    }
    else 
    {    
        return false;
    }
}

QStringList Database::getTtsExistList( const QString& engineer )
{
    QSqlQuery query;
    QStringList list;
    
    query.prepare( "SELECT SR FROM tts WHERE ENGINEER = :engineer" );
  
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer() );
    }
    else
    {
        query.bindValue( ":engineer", engineer );
    }
    
    query.exec();
    
    while( query.next() )
    {
        list.append( query.value( 0 ).toString() );
    }
    
    return list;
}

int Database::ttsAverage( const QString& engineer )
{
    QSqlQuery query;
    long long avg = 0;
    int cnt = 0;
    
    query.prepare( "SELECT TTS FROM tts WHERE ( ENGINEER = :engineer )" );
    
    if ( engineer == "NONE" )
    {
        query.bindValue( ":engineer", Settings::engineer().toUpper() );
    }
    else
    {
        query.bindValue( ":engineer", engineer.toUpper() );
    }
    
    query.exec();
    
    while( query.next() )
    {
        cnt++;
        avg = avg + query.value( 0 ).toLongLong();
    }
   
    if ( cnt > 0 )
    {
        return ( avg / cnt );
    }
    else
    {
        return 0;
    }
}

/*


                O T H E R


*/

int Database::closedTotal( const QString& engineer )
{
    return getTtsList( engineer ).count();
}

void Database::newDB( bool ask )
{
    qDebug() << "[DATABASE] Rebuild";
    
    int reply;
    
    if ( ask )
    {
        QMessageBox box;
        
        box.setWindowTitle( "Rebuild Database" );
        box.setText( "This will delete and rebuild your Database. Continue?" );
        box.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        box.setDefaultButton( QMessageBox::No );
        box.setIcon( QMessageBox::Question );
        
        reply = box.exec();
    }
    else
    {
        reply = QMessageBox::Yes;
    }

    if ( reply == QMessageBox::Yes )
    {
        mDb.close();
        QSqlDatabase::removeDatabase( mDb.connectionName() );

        QFile::remove( mDBfile );
    
        emit dbDeleted();
    }
}

QString Database::convertTime( const QString& dt )
{
    QDateTime d = QDateTime::fromString( dt, "M/d/yyyy h:mm:ss AP" );
    return ( d.toString("yyyy-MM-dd hh:mm:ss") );
}

#include "database.moc"

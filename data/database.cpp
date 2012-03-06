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

    if ( !query.exec( "CREATE TABLE IF NOT EXISTS qmon_siebel( ID INTEGER PRIMARY KEY UNIQUE, QUEUE TEXT, SEVERITY TEXT,  STATUS TEXT, "
                      "BDESC TEXT, GEO TEXT, HOURS TEXT, CUSTOMER TEXT, CONTACTVIA TEXT, CONTRACT TEXT, CREATOR TEXT, BOMGARQ TEXT, "
                      "HIGHVALUE BOOLEAN, CRITSIT BOOLEAN, AGE INTEGER, LASTACT INTEGER, TIQ INTEGER, SLA INTEGER, DISPLAY TEXT )" ) )
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
    
    QSqlQuery query( "INSERT INTO qmon_siebel( ID, QUEUE, SEVERITY, STATUS,  BDESC, GEO, HOURS, CUSTOMER, CONTACTVIA, "
                     "CONTRACT, CREATOR, BOMGARQ, HIGHVALUE, CRITSIT, AGE, LASTACT, TIQ, SLA, DISPLAY )"
                     "VALUES"
                     "( :id, :queue, :severity, :status, :bdesc, :geo, :hours, :customer, :contactvia, :contract, :creator, "
                     ":bomgarq, :highvalue, :critsit, :age, :lastact, :tiq, :sla, :display )" );
  
    query.bindValue( ":id", item->id );
    query.bindValue( ":queue", item->queue );
    query.bindValue( ":severity", item->severity );
    query.bindValue( ":status", item->status );
    query.bindValue( ":bdesc", item->bdesc );
    query.bindValue( ":geo", item->geo );
    query.bindValue( ":hours", item->hours );
    query.bindValue( ":customer", item->customer );
    query.bindValue( ":contactvia", item->contactvia );
    query.bindValue( ":contract", item->contract );
    query.bindValue( ":creator", item->creator );
    query.bindValue( ":bomgarq", item->bomgarQ );
    query.bindValue( ":highvalue", item->highValue );
    query.bindValue( ":critsit", item->critSit );
    query.bindValue( ":age", item->age );
    query.bindValue( ":lastact", item->lastAct );
    query.bindValue( ":tiq", item->timeInQ );
    query.bindValue( ":sla", item->slaLeft );
    query.bindValue( ":display", item->display );
    
    query.exec();
}

void Database::updateSiebelItemInDB( SiebelItem* item )
{
    QSqlQuery query( "UPDATE qmon_siebel SET QUEUE=:queue, SEVERITY=:severity, STATUS=:status,  BDESC=:bdesc, GEO=:geo, "
                     "HOURS=:hours, CUSTOMER=:customer, CONTACTVIA=:contactvia, CONTRACT=:contract, CREATOR=:creator, BOMGARQ=:bomgarq, "
                     "HIGHVALUE=:highvalue, CRITSIT=:critsit, AGE=:age, LASTACT=:lastact, TIQ=:tiq, SLA=:sla WHERE ID=:id" );

    query.bindValue( ":queue", item->queue );
    query.bindValue( ":severity", item->severity );
    query.bindValue( ":status", item->status );
    query.bindValue( ":bdesc", item->bdesc );
    query.bindValue( ":geo", item->geo );
    query.bindValue( ":hours", item->hours );
    query.bindValue( ":customer", item->customer );
    query.bindValue( ":contactvia", item->contactvia );
    query.bindValue( ":contract", item->contract );
    query.bindValue( ":creator", item->creator );
    query.bindValue( ":bomgarq", item->bomgarQ );
    query.bindValue( ":highvalue", item->highValue );
    query.bindValue( ":critsit", item->critSit );
    query.bindValue( ":age", item->age );
    query.bindValue( ":lastact", item->lastAct );
    query.bindValue( ":tiq", item->timeInQ );
    query.bindValue( ":sla", item->slaLeft );
    query.bindValue( ":id", item->id );
    
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

QList< SiebelItem* > Database::getSrsForQueue( const QString& queue, QString geo )
{
    QSqlQuery query;
    QList< SiebelItem* > list;
    
    if ( geo == QString::Null() )
    {
        query.prepare( "SELECT ID, QUEUE, SEVERITY, STATUS,  BDESC, GEO, HOURS, CUSTOMER, CONTACTVIA, "
                       "CONTRACT, CREATOR, BOMGARQ, HIGHVALUE, CRITSIT, AGE, LASTACT, TIQ, SLA, DISPLAY "
                       "FROM qmon_siebel WHERE ( QUEUE = :queue )" );
        
        query.bindValue( ":queue", queue );
    }
    else
    {
        query.prepare( "SELECT ID, QUEUE, SEVERITY, STATUS,  BDESC, GEO, HOURS, CUSTOMER, CONTACTVIA, "
                       "CONTRACT, CREATOR, BOMGARQ, HIGHVALUE, CRITSIT, AGE, LASTACT, TIQ, SLA, DISPLAY "
                       "FROM qmon_siebel WHERE ( QUEUE = :queue ) AND ( GEO = :geo )" );
        
        query.bindValue( ":queue", queue );
        query.bindValue( ":geo", geo );
    }
    
    query.exec();
    
    while ( query.next() ) 
    {
        SiebelItem* si = new SiebelItem;
        
        si->id = query.value( 0 ).toString();
        si->queue = query.value( 1 ).toString();
        si->severity = query.value( 2 ).toString();
        si->status = query.value( 3 ).toString();
        si->bdesc = query.value( 4 ).toString();
        si->geo = query.value( 5 ).toString();
        si->hours = query.value( 6 ).toString();
        si->customer = query.value( 7 ).toString();
        si->contactvia = query.value( 8 ).toString();
        si->contract = query.value( 9 ).toString();
        
        if ( query.value( 10 ).toString().isEmpty() )
        {
            si->isCr = false;
        }
        else
        {
            si->isCr = true;
            si->creator = query.value( 10 ).toString();
        }
        
        if ( query.value( 11 ).toString().isEmpty() )
        {
            si->isChat = false;
        }
        else
        {
            si->isChat = true;
            si->bomgarQ = query.value( 11 ).toString();
        }
        
        si->highValue = query.value( 12 ).toBool();
        si->critSit = query.value( 13 ).toBool();
        si->age = query.value( 14 ).toInt();
        si->lastAct = query.value( 15 ).toInt();
        si->timeInQ = query.value( 16 ).toInt();
        si->slaLeft = query.value( 17 ).toInt();
        si->display = query.value( 18 ).toString();
    
        list.append( si );
    }
        
    return list;
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

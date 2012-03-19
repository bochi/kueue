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
#include "queue.h"
#include "ui/html.h"

#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QDesktopServices>

void Database::openDbConnection( QString dbname )
{
    QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );

    if ( !dir.exists() )
    {
        dir.mkpath( dir.path() );
    }
    
    if ( dbname.isNull() )
    {
        dbname = "sqliteDB";
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", dbname );
    db.setDatabaseName( dir.path() + "/database.sqlite" );
        
    if ( !db.open() )
    {
        qDebug() << "[DATABASE] Failed to open the database " + dbname ;
    }
    else
    {                         
        qDebug() << "[DATABASE] Opened DB" << dir.path() << dbname;
        
        QSqlQuery query( db );
        
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
        
        if ( !query.exec("PRAGMA locking_mode = NORMAL") )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }
        
        if ( !query.exec(   "CREATE TABLE IF NOT EXISTS " + Settings::engineer().toUpper() +
                            "( ID INTEGER PRIMARY KEY UNIQUE, SRTYPE TEXT, CREATOR TEXT, CUS_ACCOUNT TEXT, "
                            "  CUS_FIRSTNAME TEXT, CUS_LASTNAME TEXT, CUS_TITLE TEXT, CUS_EMAIL TEXT, CUS_PHONE TEXT, "
                            "  CUS_ONSITEPHONE TEXT, CUS_LANG TEXT, SEVERITY TEXT, STATUS TEXT, BDESC TEXT, DDESC TEXT, "
                            "  GEO TEXT, HOURS TEXT, CONTRACT TEXT, SERVICE_LEVEL INTEGER, CREATED TEXT, LASTUPDATE TEXT, "
                            "  HIGHVALUE INTEGER, CRITSIT INTEGER, DISPLAY TEXT )" ) )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }

        if ( !query.exec( "CREATE TABLE IF NOT EXISTS qmon_siebel( ID INTEGER PRIMARY KEY UNIQUE, QUEUE TEXT, SEVERITY TEXT,  STATUS TEXT, "
                        "BDESC TEXT, GEO TEXT, HOURS TEXT, CUSTOMER TEXT, CONTACTVIA TEXT, CONTRACT TEXT, CREATOR TEXT, BOMGARQ TEXT, "
                        "HIGHVALUE BOOLEAN, CRITSIT BOOLEAN, AGE INTEGER, LASTACT INTEGER, TIQ INTEGER, SLA INTEGER, DISPLAY TEXT )" ) )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }

        if ( !query.exec(   "CREATE TABLE IF NOT EXISTS STATS "
                            "( CLOSED_SR INTEGER, CLOSED_CR INTEGER, SR_TTS_AVG INTEGER, CSAT_ENG_AVG INTEGER, "
                            "  CSAT_SR_AVG INTEGER, CSAT_RTS_PERCENT INTEGER )" ) )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }

        if ( !query.exec(   "CREATE TABLE IF NOT EXISTS STATS_SURVEYS "
                            "( ID INTEGER PRIMARY KEY UNIQUE, RTS INTEGER, ENGSAT INTEGER, SRSAT INTEGER, "
                            " CUSTOMER TEXT, BDESC TEXT )" ) )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }

        if ( !query.exec(   "CREATE TABLE IF NOT EXISTS STATS_CLOSED "
                            "( ID INTEGER PRIMARY KEY UNIQUE, TTS INTEGER, CUSTOMER TEXT, BDESC TEXT )" ) )
            
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }
        
        if ( !query.exec(   "CREATE TABLE IF NOT EXISTS QMON "
                            "( ID INTEGER PRIMARY KEY UNIQUE, QUEUE TEXT, BOMGARQ TEXT, SRTYPE TEXT, CREATOR TEXT, "
                            "  CUS_ACCOUNT TEXT, CUS_FIRSTNAME TEXT, CUS_LASTNAME TEXT, CUS_TITLE TEXT, CUS_EMAIL TEXT, "
                            "  CUS_PHONE TEXT, CUS_ONSITEPHONE TEXT, CUS_LANG TEXT, SEVERITY TEXT, STATUS TEXT, BDESC TEXT, "
                            "  DDESC TEXT, GEO TEXT, HOURS TEXT, SOURCE TEXT, SUPPORT_PROGRAM TEXT, SUPPORT_PROGRAM_LONG TEXT, "
                            "  ROUTING_PRODUCT TEXT, SUPPORT_GROUP_ROUTING TEXT, INT_TYPE TEXT, SUBTYPE TEXT, "
                            "  SERVICE_LEVEL INTEGER, CATEGORY TEXT, RESPOND_VIA TEXT, CREATED TEXT, LASTUPDATE TEXT, "
                            "  QUEUEDATE TEXT, SLA TEXT, HIGHVALUE INTEGER, CRITSIT INTEGER, DISPLAY TEXT )" ) )
        {
            qDebug() << "[DATABASE] Error:" << query.lastError();
        }
    }
}


/*


                P E R S O N A L   Q U E U E


*/


void Database::getSRData( SR* sr, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    
    QSqlQuery query( db );
    query.prepare( "SELECT ID, CDATE, ADATE, STATUS, CUSTOMER, CONTACT, BDESC, DDESC, SS, DISPLAY "
                   "FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
    
    query.bindValue( ":id", sr->id() );
    query.exec();

    if ( query.next() )
    {
        /*sr->setOpened( QDateTime::fromString( query.value( 1 ).toString(), "yyyy-MM-dd hh:mm:ss" ) );
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
        sr->setLastUpdateDays( sr->lastUpdate().daysTo( now ) );*/

    }
//    else qDebug() << "[DATABASE] Failed to get SR data for" << sr->id() << query.executedQuery() << query.lastError();
}

void Database::updateSRData( QueueSR sr, const QString& dbname )
{   
    QSqlDatabase db = QSqlDatabase::database( dbname );
    
    QSqlQuery query(db);
    
    qDebug() << "[DATABASE] Updating SR data for" << sr.id;
    
    query.prepare(  "UPDATE " + Settings::engineer().toUpper() + " SET "
                    "SRTYPE=:srtype, CREATOR=:creator, CUS_ACCOUNT=:cus_account, CUS_FIRSTNAME=:cus_firstname, "
                    "CUS_LASTNAME=:cus_lastname, CUS_TITLE=:cus_title, CUS_EMAIL=:cus_email, CUS_PHONE=:cus_phone, "
                    "CUS_ONSITEPHONE=:cus_onsitephone, CUS_LANG=:cus_lang, SEVERITY=:severity, STATUS=:status, "
                    "BDESC=:bdesc, DDESC=:ddesc, GEO=:geo, HOURS=:hours, CONTRACT=:contract, SERVICE_LEVEL=:service_level, "
                    "CREATED=:created, LASTUPDATE=:lastupdate, HIGHVALUE=:highvalue, CRITSIT=:critsit WHERE ID = :id" );
    
    query.bindValue( ":srtype", sr.srtype );
    query.bindValue( ":creator", sr.creator );
    query.bindValue( ":cus_account", sr.cus_account );
    query.bindValue( ":cus_firstname", sr.cus_firstname );
    query.bindValue( ":cus_lastname", sr.cus_lastname );
    query.bindValue( ":cus_title", sr.cus_title );
    query.bindValue( ":cus_email", sr.cus_email );
    query.bindValue( ":cus_phone", sr.cus_phone );
    query.bindValue( ":cus_onsitephone", sr.cus_onsitephone );
    query.bindValue( ":cus_lang", sr.cus_lang );
    query.bindValue( ":severity", sr.severity );
    query.bindValue( ":status", sr.status );
    query.bindValue( ":bdesc", sr.bdesc );
    query.bindValue( ":ddesc", sr.ddesc );
    query.bindValue( ":geo", sr.geo );
    query.bindValue( ":hours", sr.hours );
    query.bindValue( ":contract", sr.contract );
    query.bindValue( ":service_level", sr.service_level );
    query.bindValue( ":created", sr.created );
    query.bindValue( ":lastupdate", sr.lastupdate );
    query.bindValue( ":highvalue", sr.highvalue );
    query.bindValue( ":critsit", sr.critsit );
    query.bindValue( ":id", sr.id );

    if ( !query.exec() ) 
    {
        qDebug() << "[DATABASE] Failed to update" << sr.id << "in DB" << query.executedQuery() << query.lastError();
    }
}

void Database::insertSRData( QueueSR sr, const QString& dbname )
{
    qDebug() << "[DATABASE] Inserting SR data for" << sr.id;
    
    QSqlDatabase db = QSqlDatabase::database( dbname );
    
    QSqlQuery query( db );
    
    query.prepare(  "INSERT INTO " + Settings::engineer().toUpper() +
                    "( ID, SRTYPE, CREATOR, CUS_ACCOUNT, CUS_FIRSTNAME, CUS_LASTNAME, CUS_TITLE, CUS_EMAIL, CUS_PHONE, "
                    "  CUS_ONSITEPHONE, CUS_LANG, SEVERITY, STATUS, BDESC, DDESC, GEO, HOURS, CONTRACT, SERVICE_LEVEL, "
                    "  CREATED, LASTUPDATE, HIGHVALUE, CRITSIT, DISPLAY )"
                    "VALUES" 
                    "( :id, :srtype, :creator, :cus_account, :cus_firstname, :cus_lastname, :cus_title, :cus_email, :cus_phone, "
                    "  :cus_onsitephone, :cus_lang, :severity, :status, :bdesc, :ddesc, :geo, :hours, :contract, :service_level, "
                    "  :created, :lastupdate, :highvalue, :critsit, 'none' )" );
        
    query.bindValue( ":id", sr.id );
    query.bindValue( ":srtype", sr.srtype );
    query.bindValue( ":creator", sr.creator );
    query.bindValue( ":cus_account", sr.cus_account );
    query.bindValue( ":cus_firstname", sr.cus_firstname );
    query.bindValue( ":cus_lastname", sr.cus_lastname );
    query.bindValue( ":cus_title", sr.cus_title );
    query.bindValue( ":cus_email", sr.cus_email );
    query.bindValue( ":cus_phone", sr.cus_phone );
    query.bindValue( ":cus_onsitephone", sr.cus_onsitephone );
    query.bindValue( ":cus_lang", sr.cus_lang );
    query.bindValue( ":severity", sr.severity );
    query.bindValue( ":status", sr.status );
    query.bindValue( ":bdesc", sr.bdesc );
    query.bindValue( ":ddesc", sr.ddesc );
    query.bindValue( ":geo", sr.geo );
    query.bindValue( ":hours", sr.hours );
    query.bindValue( ":contract", sr.contract );
    query.bindValue( ":service_level", sr.service_level );
    query.bindValue( ":created", sr.created );
    query.bindValue( ":lastupdate", sr.lastupdate );
    query.bindValue( ":highvalue", sr.highvalue );
    query.bindValue( ":critsit", sr.critsit );
    
    if ( !query.exec() ) 
    {
        qDebug() << "[DATABASE] Failed to add" << sr.id << "into DB" << query.executedQuery() << query.lastError();
    }
}

void Database::deleteSrFromDB( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );

    qDebug() << "[DATABASE] Deleting SR" << id;
    
    QSqlQuery query(db);
    query.prepare( "DELETE FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
}

bool Database::srExistsInDB( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    
    QSqlQuery query(db);
    
    query.prepare( "SELECT ID FROM " + Settings::engineer().toUpper() +" WHERE ( ID = :id )" );
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

bool Database::srWasUpdated( QueueSR sr, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    
    query.prepare( "SELECT LASTUPDATE FROM " + Settings::engineer().toUpper() + " WHERE ( ID = :id )" );
    query.bindValue( ":id", sr.id );
    
    query.exec();
    
    if ( query.next() )
    {
        if ( sr.lastupdate != query.value( 0 ).toString() )
        {
            return true;
        }
    }
    
    return false;
}


QString Database::getAdate( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT ADATE FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

QString Database::getStatus( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT STATUS FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

void Database::setDisplay( const QString& d, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    
    query.prepare(  "UPDATE " + Settings::engineer().toUpper() + " SET "
                    "DISPLAY=:display "
                    "WHERE id = :id" );
    
    query.bindValue( ":display", d.split( "-" ).at( 1 ) );
    query.bindValue( ":id", d.split( "-" ).at( 0 ) );
    query.exec();
}

void Database::closeAllTables( const QString& dbname)
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    
    query.prepare(  "UPDATE " + Settings::engineer().toUpper() + " SET "
                    "DISPLAY='none' " );
        
    query.exec();
}

void Database::expandAllTables( const QString& dbname)
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    
    query.prepare(  "UPDATE " + Settings::engineer().toUpper() + " SET "
                    "DISPLAY='block' " );
    
    query.exec();
}

QList<QueueSR> Database::getSrList( bool s, bool a, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QStringList l;
    QDateTime now = QDateTime::currentDateTime();
    QList<QueueSR> srlist;

    QSqlQuery query( db );
    
    db.transaction();

    if ( a )
    {
        if ( s ) 
        {    
            qDebug() << "ORDER BY LASTUPDATE ASC";
            query.prepare(  "SELECT ID, SRTYPE, CREATOR, CUS_ACCOUNT, CUS_FIRSTNAME, CUS_LASTNAME, CUS_TITLE, CUS_EMAIL, CUS_PHONE, "
                            "CUS_ONSITEPHONE, CUS_LANG, SEVERITY, STATUS, BDESC, DDESC, GEO, HOURS, CONTRACT, SERVICE_LEVEL, "
                            "CREATED, LASTUPDATE, HIGHVALUE, CRITSIT, DISPLAY FROM " + Settings::engineer().toUpper() + " ORDER BY CREATED ASC" );
        }
        else 
        {   
            qDebug() << "ORDER BY LASTUPDATE SC";
            query.prepare(  "SELECT ID, SRTYPE, CREATOR, CUS_ACCOUNT, CUS_FIRSTNAME, CUS_LASTNAME, CUS_TITLE, CUS_EMAIL, CUS_PHONE, "
                            "CUS_ONSITEPHONE, CUS_LANG, SEVERITY, STATUS, BDESC, DDESC, GEO, HOURS, CONTRACT, SERVICE_LEVEL, "
                            "CREATED, LASTUPDATE, HIGHVALUE, CRITSIT, DISPLAY FROM " + Settings::engineer().toUpper() + " ORDER BY LASTUPDATE ASC" );
        }
    }
    else
    {
        if ( s ) 
        {    
            qDebug() << "ORDER BY CREATED ASC";
            query.prepare(  "SELECT ID, SRTYPE, CREATOR, CUS_ACCOUNT, CUS_FIRSTNAME, CUS_LASTNAME, CUS_TITLE, CUS_EMAIL, CUS_PHONE, "
                            "CUS_ONSITEPHONE, CUS_LANG, SEVERITY, STATUS, BDESC, DDESC, GEO, HOURS, CONTRACT, SERVICE_LEVEL, "
                            "CREATED, LASTUPDATE, HIGHVALUE, CRITSIT, DISPLAY FROM " + Settings::engineer().toUpper() + " ORDER BY CREATED DESC" );
        }
        else 
        {    
            qDebug() << "ORDER BY CREATED DESC";
            query.prepare(  "SELECT ID, SRTYPE, CREATOR, CUS_ACCOUNT, CUS_FIRSTNAME, CUS_LASTNAME, CUS_TITLE, CUS_EMAIL, CUS_PHONE, "
                            "CUS_ONSITEPHONE, CUS_LANG, SEVERITY, STATUS, BDESC, DDESC, GEO, HOURS, CONTRACT, SERVICE_LEVEL, "
                            "CREATED, LASTUPDATE, HIGHVALUE, CRITSIT, DISPLAY FROM " + Settings::engineer().toUpper() + " ORDER BY LASTUPDATE DESC" );
        }
    }
        
    query.exec();
    
    while( query.next() )
    {
        QueueSR sr;
        
        sr.id = query.value(0).toString();
        sr.srtype = query.value(1).toString();
        sr.creator = query.value(2).toString();
        sr.cus_account = query.value(3).toString();
        sr.cus_firstname = query.value(4).toString();
        sr.cus_lastname = query.value(5).toString();
        sr.cus_title = query.value(6).toString();
        sr.cus_email = query.value(7).toString();
        sr.cus_phone = query.value(8).toString();
        sr.cus_onsitephone = query.value(9).toString();
        sr.cus_lang = query.value(10).toString();
        sr.severity = query.value(11).toString();
        sr.status = query.value(12).toString();
        sr.bdesc = query.value(13).toString();
        sr.ddesc = query.value(14).toString();
        sr.geo = query.value(15).toString();
        sr.hours = query.value(16).toString();
        sr.contract = query.value(17).toString();
        sr.service_level = query.value(18).toInt();
        sr.created = query.value(19).toString();
        sr.lastupdate = query.value(20).toString();
        sr.highvalue = query.value(21).toBool();
        sr.critsit = query.value(22).toBool();
        sr.display = query.value(23).toString();
        
        QDateTime a = QDateTime::fromString( sr.created, "yyyy-MM-dd hh:mm:ss" );
        QDateTime u = QDateTime::fromString( sr.lastupdate, "yyyy-MM-dd hh:mm:ss" );
        
        sr.age = a.daysTo( now );
        sr.lastUpdateDays = u.daysTo( now );
     
        QStringList todo;
        
        if ( Settings::todoShowStat() )
        {
            if ( sr.status == "Assigned" || sr.status == "Unassigned" ) 
            {
                todo.append( "[2]The SR is in status " + sr.status + ", that's probably incorrect. Please check and change status if needed." );
            }
        }
      
        if ( Settings::todoShowUp() )
        {
            if ( sr.status == "Awaiting Novell Support" && sr.lastUpdateDays >= 3 ) 
            {
                todo.append( "[1]There was no activity for " + QString::number( sr.lastUpdateDays ) + " days and the customer is waiting for Novell, please update SR asap!" );
            }
        }
        else if ( sr.status != "Suspended" && sr.lastUpdateDays >= 5 ) 
        {    
            todo.append( "[1]There was no activity for " + QString::number( sr.lastUpdateDays ) + " days, please update SR asap!" );
        }
      
        if ( todo.isEmpty() ) 
        {
            todo.append( "[3]Nothing." ); 
        }
        
        sr.todoList = todo;
        
        srlist.append( sr );
    }
    
    db.commit();
    
    return srlist;
}

QStringList Database::getSrNrList( const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query( db );
    QStringList list;
    
    query.prepare( "SELECT ID FROM " + Settings::engineer().toUpper() );
    
    while ( query.next() )
    {
        list.append( query.value( 0 ).toString() );
    }
    
    return list;
}

int Database::srsTotal( const QString& dbname)
{
    return getSrList( true, true, dbname ).count();
}

QString Database::getDetailedDescription( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT DDESC FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

QString Database::getSrStatus( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT STATUS FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

QString Database::getCustomer( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT CUSTOMER, CONTACT FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

QString Database::getBriefDescription( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    query.prepare( "SELECT BDESC FROM " + Settings::engineer().toUpper() + " WHERE ID = :id" );
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

void Database::insertSiebelItemIntoDB( SiebelItem* item, const QString& dbname )
{
    qDebug() << "[DATABASE] Inserting SiebelItem for " << item->id << item->queue;
    
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query( "INSERT INTO qmon_siebel( ID, QUEUE, SEVERITY, STATUS,  BDESC, GEO, HOURS, CUSTOMER, CONTACTVIA, "
                     "CONTRACT, CREATOR, BOMGARQ, HIGHVALUE, CRITSIT, AGE, LASTACT, TIQ, SLA, DISPLAY )"
                     "VALUES"
                     "( :id, :queue, :severity, :status, :bdesc, :geo, :hours, :customer, :contactvia, :contract, :creator, "
                     ":bomgarq, :highvalue, :critsit, :age, :lastact, :tiq, :sla, :display )", db );
  
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

void Database::updateSiebelItemInDB( SiebelItem* item, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query( "UPDATE qmon_siebel SET QUEUE=:queue, SEVERITY=:severity, STATUS=:status,  BDESC=:bdesc, GEO=:geo, "
                     "HOURS=:hours, CUSTOMER=:customer, CONTACTVIA=:contactvia, CONTRACT=:contract, CREATOR=:creator, BOMGARQ=:bomgarq, "
                     "HIGHVALUE=:highvalue, CRITSIT=:critsit, AGE=:age, LASTACT=:lastact, TIQ=:tiq, SLA=:sla WHERE ID=:id", db );

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


void Database::updateSiebelDisplay( const QString& display, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query( "UPDATE qmon_siebel SET DISPLAY = :display WHERE id = :id" );
                
    query.bindValue( ":display", display.split( "-" ).at( 1 ) );
    query.bindValue( ":id", display.split( "-" ).at( 0 ) );
                
    query.exec();   
}

void Database::deleteSiebelItemFromDB( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    qDebug() << "[DATABASE] Deleting SiebelItem" << id;
    
    QSqlQuery query(db);
    query.prepare( "DELETE FROM qmon_siebel WHERE ID = :id" );
    query.bindValue( ":id", id );
    query.exec();
}

QStringList Database::getQmonSiebelList( const QString& dbname)
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
    QStringList l;
    query.prepare( "SELECT ID FROM qmon_siebel" );
    query.exec();
    
    while( query.next() )
    {
        l.append( query.value( 0 ).toString() );
    }
    
    return l;    
}

bool Database::siebelExistsInDB( const QString& id, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
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

QList< SiebelItem* > Database::getSrsForQueue( const QString& queue, QString geo, const QString& dbname )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
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

bool Database::siebelQueueChanged( SiebelItem* si, const QString& dbname  )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
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

bool Database::siebelSeverityChanged( SiebelItem* si, const QString& dbname  )
{
    QSqlDatabase db = QSqlDatabase::database( dbname );
    QSqlQuery query(db);
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


/*void Database::newDB( bool ask )
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
}*/

QString Database::convertTime( const QString& dt )
{
    QDateTime d = QDateTime::fromString( dt, "M/d/yyyy h:mm:ss AP" );
    return ( d.toString("yyyy-MM-dd hh:mm:ss") );
}

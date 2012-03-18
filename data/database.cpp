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

void Database::openDbConnection( QString dbname )
{
    QDir dir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );

    if ( !dir.exists() )
    {
        dir.mkpath( dir.path() );
    }
    
    if ( dbname.isNull() )
    {
        dbname = "default";
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", dbname );
    db.setDatabaseName( dir.path() + "/database.sqlite" );
        
    if ( !db.open() )
    {
        qDebug() << "[DATABASE] Failed to open the database " + dbname ;
    }
    else
    {                         
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
    QSqlQuery query;
    query.prepare( "SELECT ID, CDATE, ADATE, STATUS, CUSTOMER, CONTACT, BDESC, DDESC, SS, DISPLAY "
                   "FROM " + Settings::engineer() + " WHERE ID = :id" );
    
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

void Database::updateSRData( SR* sr )
{    
    QSqlQuery query;
    
    qDebug() << "[DATABASE] Updating SR data for" << sr->id();
    
    query.prepare(  "UPDATE " + Settings::engineer() + " SET "
                    "SRTYPE=:srtype, CREATOR=:creator, CUS_ACCOUNT=:cus_account, CUS_FIRSTNAME=:cus_firstname, "
                    "CUS_LASTNAME=:cus_lastname, CUS_TITLE=:cus_title, CUS_EMAIL=:cus_email, CUS_PHONE=:cus_phone, "
                    "CUS_ONSITEPHONE=:cus_onsitephone, CUS_LANG=:cus_lang, SEVERITY=:severity, STATUS=:status, "
                    "BDESC=:bdesc, DDESC=:ddesc, GEO=:geo, HOURS=:hours, CONTRACT=:contract, SERVICE_LEVEL=:service_level, "
                    "CREATED=:created, LASTUPDATE=:lastupdate, HIGHVALUE=:highvalue, CRITSIT=:critsit WHERE ID = :id" );

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

bool Database::srWasUpdated( QueueSR sr )
{
    QSqlQuery query;
    
    query.prepare( "SELECT LASTUPDATE FROM " + Settings::engineer() + " WHERE ( ID = :id )" );
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

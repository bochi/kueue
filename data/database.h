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

#ifndef DATABASE_H_
#define DATABASE_H_

#include "data/dataclasses.h"
#include "settings.h"

#include <QtSql>

class Database
{
    public:
        static void openDbConnection( const QString = QString::Null() );
        
        static bool srWasUpdated( QueueSR, const QString& = "sqliteDB" );
   
        static QString getBriefDescription( const QString&, const QString& = "sqliteDB" );
        static QString getDetailedDescription( const QString&, const QString& = "sqliteDB" );
        static QString getCustomer( const QString&, const QString& = "sqliteDB" );
        static QString getCreator( const QString&, const QString& = "sqliteDB" );
        static QString getStatus( const QString&, const QString& = "sqliteDB" );
        static bool isCr( const QString&, const QString& = "sqliteDB" );
             
        static void setQueueDisplay( const QString&, const QString& = "sqliteDB" );
        static void setQmonDisplay( const QString&, const QString& = "sqliteDB" );
        static void closeAllTables( const QString& = "sqliteDB");
        static void expandAllTables( const QString& = "sqliteDB");
        
        static QList<Notification> updateQueue( PersonalQueue, const QString& = "sqliteDB" );
        static void insertQueueSR( QueueSR, const QString& = "sqliteDB" );
        static void updateQueueSR( QueueSR, const QString& = "sqliteDB" );
        static void deleteQueueSR( const QString&, const QString& = "sqliteDB" );
        static QStringList getSrNrList( const QString& = "sqliteDB" );
        static QList<QueueSR> getSrList( bool = true, bool = true, const QString& = "sqliteDB", const QString& = QString::Null() );
        static QueueSR getSrInfo( const QString&, const QString& = "sqliteDB" );
        static bool queueSrExists( const QString&, const QString& = "sqliteDB" );
        
        static void updateStats( Statz, const QString& = "sqliteDB" );
        static void insertSurvey( Survey, const QString& = "sqliteDB" );
        static void insertClosed( ClosedItem, const QString& = "sqliteDB" );
        static Statz getStatz( const QString& = "sqliteDB" );
        
        static QList<Notification> updateQmon( QmonData, const QString& = "sqliteDB" );
        static void dropQmon( const QString& = "sqliteDB" );
        static void insertQmonSR( QmonSR, const QString& = "sqliteDB" );
        static void updateQmonSR( QmonSR, const QString& = "sqliteDB" );
        static bool qmonExists( const QString&, const QString& = "sqliteDB" );
        static bool qmonQueueChanged( QmonSR, const QString& = "sqliteDB" );
        static bool qmonSeverityChanged( QmonSR, const QString& = "sqliteDB" );
        static void deleteQmonSR( const QString&, const QString& = "sqliteDB" );
        static QStringList getQmonSrNrs( const QString& = "sqliteDB" );
        static QList< QmonSR > getQmonQueue( const QString&, QString = QString::Null(), const QString& = "sqliteDB",  const QString& = QString::Null() );
        
        static void addGoneSR( const QString&, const QString& = "sqliteDB" );
        static void delGoneSR( const QString&, const QString& = "sqliteDB" );
        static QStringList getGoneSRs( const QString& = "sqliteDB" );
        static int getGoneDays( const QString&, const QString& = "sqliteDB" );
                       
        static QString convertTime( const QString& );
        
        static void newDB( bool = false );
        
    private:
        QSqlDatabase mDb;
        QString mDBfile;
   
    public slots:
        //void newDB( bool = true );
                        
    signals:
        void dbDeleted();
};

#endif

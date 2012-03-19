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

#include "data/qmon.h"
#include "data/stats.h"
#include "data/sr.h"
#include "data/dataclasses.h"

#include <QtSql>

class Database
{
    public:
        static void openDbConnection( const QString = QString::Null() );
        
        static bool srWasUpdated( QueueSR, const QString& = "sqliteDB" );
        
        static void getSRData( SR*, const QString& = "sqliteDB" );
        static void updateSRData( QueueSR, const QString& = "sqliteDB" );
        static void insertSRData( QueueSR, const QString& = "sqliteDB" );
        static void deleteSrFromDB( const QString&, const QString& = "sqliteDB" );
        static bool srExistsInDB( const QString&, const QString& = "sqliteDB" );
        static QString getAdate( const QString&, const QString& = "sqliteDB" );
        
        static QString getStatus( const QString&, const QString& = "sqliteDB" );
        static void setDisplay( const QString&, const QString& = "sqliteDB" );
        static void closeAllTables( const QString& = "sqliteDB");
        static void expandAllTables( const QString& = "sqliteDB");
        static QList<QueueSR> getSrList( bool = true, bool = true, const QString& = "sqliteDB" );
        static QStringList getSrNrList( const QString& = "sqliteDB" );
        static int srsTotal( const QString& = "sqliteDB");
        static QString getBriefDescription( const QString&, const QString& = "sqliteDB" );
        static QString getDetailedDescription( const QString&, const QString& = "sqliteDB" );
        static QString getCustomer( const QString&, const QString& = "sqliteDB" );
        static QString getSrStatus( const QString&, const QString& = "sqliteDB" );
        
        static void insertSiebelItemIntoDB( SiebelItem*, const QString& = "sqliteDB" );
        static void updateSiebelItemInDB( SiebelItem*, const QString& = "sqliteDB" );
        static void updateSiebelQueue( SiebelItem*, const QString& = "sqliteDB" );
        static void updateSiebelSeverity( SiebelItem*, const QString& = "sqliteDB" );
        static void updateSiebelDisplay( const QString&, const QString& = "sqliteDB" );
        static void deleteSiebelItemFromDB( const QString&, const QString& = "sqliteDB" );
        static QStringList getQmonSiebelList( const QString& = "sqliteDB");
        static bool siebelExistsInDB( const QString&, const QString& = "sqliteDB" );
        static bool siebelQueueChanged( SiebelItem*, const QString& = "sqliteDB" );
        static bool siebelSeverityChanged( SiebelItem*, const QString& = "sqliteDB" );
        static bool isChat( const QString&, const QString& = "sqliteDB" );
        static QString getQmonBdesc( const QString&, const QString& = "sqliteDB" );
        
        static QList< SiebelItem* > getSrsForQueue( const QString&, QString = QString::Null(), const QString& = "sqliteDB" );
               
        static QString convertTime( const QString& );
        
    private:
        QSqlDatabase mDb;
        QString mDBfile;
   
    public slots:
        //void newDB( bool = true );
                        
    signals:
        void dbDeleted();
};

#endif

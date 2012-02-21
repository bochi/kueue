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

#ifndef DATABASE_H_
#define DATABASE_H_

#include "data/qmon.h"
#include "data/stats.h"
#include "data/sr.h"

#include <QtSql>
#include <QThread>

class Database : public QThread
{
    Q_OBJECT

    public: 
        Database();
        ~Database();
        
        static void getSRData( SR* );
        static void updateSRData( SR* );
        static void insertSRData( SR* );
        static void deleteSrFromDB( const QString& );
        static bool srExistsInDB( const QString& );
        static QString getAdate( const QString& );
        static QString getStatus( const QString& );
        static void setDisplay( const QString& );
        static void closeAllTables();
        static void expandAllTables();
        static QStringList getSrList( bool = true, bool = true );
        static int srsTotal();
        static QString getBriefDescription( const QString& );
        static QString getDetailedDescription( const QString& );
        static QString getCustomer( const QString& );
        static QString getSrStatus( const QString& );
        
        static void insertSiebelItemIntoDB( SiebelItem* );
        static void updateSiebelItemInDB( SiebelItem* );
        static void updateSiebelQueue( SiebelItem* );
        static void updateSiebelSeverity( SiebelItem* );
        static void updateSiebelDisplay( const QString& );
        static void deleteSiebelItemFromDB( const QString& );
        static QStringList getQmonSiebelList();
        static bool siebelExistsInDB( const QString& );
        static bool siebelQueueChanged( SiebelItem* );
        static bool siebelSeverityChanged( SiebelItem* );
        static bool isChat( const QString& );
        static QString getQmonBdesc( const QString& );
        
        static void updateBomgarItemInDB( BomgarItem* );
        static void deleteBomgarItemFromDB( const QString& );
        static QList< SiebelItem* > getSrsForQueue( const QString&, QString = QString::Null() );
        static QStringList getQmonBomgarList();
        static bool bomgarExistsInDB( const QString& );
        static QString getBomgarQueue( const QString& );
        static QString getBomgarQueueById( const QString& );
        static void updateBomgarQueue( BomgarItem* );
        
        static void updateCsatData( CsatItem* );
        static void deleteCsatItemFromDB( const QString&, const QString& );
        static bool csatExistsInDB( const QString&, const QString& );
        static QList< CsatItem* > getCsatList( const QString& = "NONE" );
        static QStringList getCsatExistList();
        static int csatEngAverage( const QString& = "NONE" );
        static int csatSrAverage( const QString& = "NONE" );
        static int csatRtsPercent( const QString& = "NONE" );
        
        static void updateTtsData( TtsItem* );
        static void deleteTtsItemFromDB( const QString& );
        static bool ttsExistsInDB( const QString& );
        static QStringList getTtsExistList( const QString& engineer = "NONE" );
        static QList< TtsItem* > getTtsList( const QString& = "NONE" );
        static int ttsAverage( const QString& = "NONE" );
        
        static QString getWfid( const QString& );
        static QString getEngineerForWfid( const QString& );
        static void updateWFID( const QString&, const QString& );

        static int closedTotal( const QString& = "NONE" );
        
        static QString convertTime( const QString& );
        
    private:
        QSqlDatabase mDb;
        QString mDBfile;
   
    public slots:
        void newDB( bool = true );
                        
    signals:
        void dbDeleted();
};

#endif

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

#ifndef NSA_H_
#define NSA_H_

#include <QtSql>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QProgressDialog>
#include <QtWebKit>

#include "kueuethreads.h"
#include "nsa/nsawindow.h"

class NSATableItem;
class KueueThreads;

class NSA : public QObject
{
    Q_OBJECT

    public: 
        NSA( const QString&, const QString& = QString::Null() );
        ~NSA();
        
    private:
        QProgressDialog* mProgress;
        QWebView* mNSAView;
        QDir mNsaDir;
        QDir mTmpDir;
        QFile mOutputFile;
        QString mReport;
        QString mSupportConfig;
        QString mCritical;
        QString mWarning;
        QString mRecommended;
        QString mSaveLocation;
        KueueThreads* mThreads;
        bool mIsArchive;
      
    private slots:
        void updateNsaData();
        void updateDownloadDone();
        void fileDownloadDone();
        void startReport();
        void runPS( const QString&, const QString& );
        void ranPS( const QString& );
        void winClosed( NSAWindow* );
        
    protected: 
        void run();
        
    signals:
        void startedReport( const QString&, int );
        void nsaReportFinished();
        
};


#endif

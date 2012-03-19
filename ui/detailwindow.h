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

#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H
 
#include "ui_detailwindow.h"

#include <QNetworkReply>
#include <QProgressDialog>
#include <QShortcut>
 
class DetailWindow : public QMainWindow, private Ui::DetailWindowDLG
{

    Q_OBJECT
		     
    public:
        DetailWindow( QString sr = 0, bool = false );
	~DetailWindow();
        
    private:
	QString mAssignResult;
        QString mSr;
        QString mDetails;
        QString mSrDetails;
        QString mEngineer;
        QNetworkReply* mDet1;
        QNetworkReply* mDet2;
        QNetworkReply* mAssign;
        QProgressDialog* mProgress;
        QShortcut* mTransSC;
        
    private slots:
        void downloadDetails();
        void translate();
        void detailFinished();
        void detail2Finished();
        void transaction_done();
	void takePressed();
        void assignSR();
        void showProgress( const QString& );
        void assignJobDone();
        void assignNow();
        void sendWithKopete();
        void closeEvent( QCloseEvent* );

    signals:
        void toClipboard( const QString& );
	void done( DetailWindow* );
};
 
#endif

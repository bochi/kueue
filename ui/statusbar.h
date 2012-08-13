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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QToolButton>

#include "unity/unitynetwork.h"
#include "ui/download/downloadmanager.h"

class StatusBar : public QStatusBar
{
    Q_OBJECT

    private:
        static StatusBar* instance;
        StatusBar();
        StatusBar(const StatusBar&) {}
        ~StatusBar();
   
        QProgressBar* mProgress;
        QLabel* mLabel;
        QWidget* mDownloadWidget;
        void addDownloadJobImpl( QNetworkReply*, QString, bool );
        void addDownloadJobImpl( QNetworkRequest, QNetworkAccessManager*, QString, bool );      
        void startJobStatusImpl( const QString&, int );
        
        DownloadManager* mDownloadManager;
        QToolButton* mDownloadButton;
        QToolButton* mThreadButton;
        
    public:
        static StatusBar& getInstance();
        static void destroy();
        static void addDownloadJob( QNetworkRequest r, QNetworkAccessManager* m, QString s, bool a )
        {
            instance->addDownloadJobImpl( r, m, s, a );
        }
        static void addDownloadJob( QNetworkReply* r, QString s, bool a )
        {
            instance->addDownloadJobImpl( r, s, a );
        }
        static void hideDownloadManager()
        {
            instance->hideDownloadManagerImpl();
        }
        
        static void addThreadWidget( QWidget* w )
        {
            instance->addWidgetImpl( w );
        }
        
        static void removeThreadWidget( QWidget* w )
        {
            instance->removeWidgetImpl( w );
        }
        
        static void showMessage( const QString& msg, int timeout = 15000 )
        {
            instance->showMessageImpl( msg, timeout );
        }
        
    private slots:
        void undefinedDownload();
        void toggleDownloadManager();
        void showDownloadManager();
        void hideDownloadManagerImpl();
        void popupDownloadManager();
        void mousePressEvent( QMouseEvent* );
        void addWidgetImpl( QWidget* );
        void removeWidgetImpl( QWidget* );
        void showMessageImpl( const QString&, int = 15000 );
        void resetStatusbar();
    
};

#endif

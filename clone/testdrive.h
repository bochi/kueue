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

#ifndef TESTDRIVE_H
#define TESTDRIVE_H

#include "kueuethreads.h"
#include "dataclasses.h"
#include "qstudio.h"
#include "vncview.h"
#include "ui/vncwidget.h"

class TestDriveThread;
class TestDriveWorker;

class TestDrive : public QObject
{
    Q_OBJECT
    
    public: 
        TestDrive( int, const QString& );
        ~TestDrive();
        
        VncWidget* widget() { return mVncWidget; }
        int tabId() { return mTabId; }
              
    public slots:
        void setTabId( int );

    private slots:
        void closeTestdrive();
        
    private:
        VncWidget* mVncWidget;
        TestDriveThread* mThread;
        int mTabId;
        
    signals:
        void downloadRequested( QString );
        void closeRequested( int );
};

class TestDriveThread : public QThread
{
    Q_OBJECT

    public: 
        TestDriveThread( int );
        ~TestDriveThread();
        
    public slots:
        void requestNewTestdrive();
        void deleteWorker();
        void downloadAppliance();

    private:
        int mBuildId;
        TestDriveWorker* mWorker;
        
    protected: 
        void run();
        
    signals:
        void vnc( QUrl );
        void newTestdriveRequested();
        void timedOut( int );
        void threadKilled( int );
        void downloadApplianceRequested();
        void downloadRequested( QString );
};

class TestDriveWorker : public QObject
{
    Q_OBJECT

    public: 
        TestDriveWorker( int );
        ~TestDriveWorker();
        
    private:
        int mBuildId;
        int mTestDriveId;
        QTimer* mTimer;
        QStudio* mStudio;
        QUrl mUrl;
        
    public slots:
        void work();
        void newTestdriveRequested();
        void downloadAppliance();
        
    private slots:
        void getTestdriveForBuild();
        void checkTestdrive();
        
    signals:
        void vnc( QUrl );
        void timedOut( int );
        void downloadRequested( QString );
};

#endif
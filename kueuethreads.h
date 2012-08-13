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

#ifndef KUEUETHREADS_H
#define KUEUETHREADS_H

#include <QObject>
#include <QList>
#include <QThread>

#include "config.h"
#include "ui/statusbar.h"
#include "nsa/nsa.h"

class KueueThread;

class KueueThreads : public QObject
{
    Q_OBJECT

    public:
        static KueueThreads& getInstance();
        static void destroy();
        static void enqueue( KueueThread* thread )
        {
            getInstance().enqueueThread( thread );
        }
        
        static QWidget* threadWidget()
        { 
            return getInstance().getThreadWidget();
        }
        
    private:
        static KueueThreads* instance;
        KueueThreads( QObject* parent = 0 );
        ~KueueThreads();
        KueueThread* mCurrentThread;
        QList<QWidget*> mThreadWidgetList;
        QWidget* getThreadWidget();
        QWidget* mThreadWidget;
        
    protected:
        QList<KueueThread*> mThreadList;
        StatusBar* mStatusBar;
        void enqueueThread( KueueThread* );
        
    private slots:
        void next();
        void startThread( const QString&, int );
        void updateThreadProgress( int, const QString& );
        void endThread();
        void addThreadWidget( QWidget* );
        void removeThreadWidget( QWidget* );
};

class KueueThread : public QThread
{
    Q_OBJECT
    
    public:
        KueueThread( QObject* parent = 0 );
        ~KueueThread();
        
    signals:
        void threadStarted( const QString&, int );
        void threadNewMaximum( int );
        void threadProgress( int, const QString& );
        void threadFinished( KueueThread* );
};

#endif

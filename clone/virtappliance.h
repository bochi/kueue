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

#ifndef VIRTAPPLIANCE_H
#define VIRTAPPLIANCE_H

#include "kueuethreads.h"
#include "ui/vncwidget.h"
#include "qvirt/qvirt.h"

#include <QProcess>

class KueueThreads;
class VirtApplianceThread;
class VirtApplianceWorker;

class VirtAppliance : public QObject
{
    Q_OBJECT

    public: 
        VirtAppliance( const QString&, const QString& );
        ~VirtAppliance();
        
        int tabId() { return mTabId; }
        VncWidget* widget() { return mVncWidget; }
        
    private:
        VncWidget* mVncWidget;
        VirtApplianceThread* mThread;
        int mTabId;
        
    public slots:
        void setTabId( int );
        
    private slots:
        void domainExists( const QString& );
        
    signals:
    
};

class VirtApplianceThread : public QThread
{
    Q_OBJECT

    public: 
        VirtApplianceThread( const QString&, const QString& );
        ~VirtApplianceThread();
        
    public slots:
        void deleteWorker();
        void recreateDomain( const QString& );
        void connectToInstance( const QString& );

    private:
        VirtApplianceWorker* mWorker;
        QString mVmdk;
        QString mName;
        
    protected: 
        void run();
        
    signals:
        void vnc( QUrl );
        void domainExists( QString );
        void recreateDomainRequested( QString );
        void connectToInstanceRequested( QString );
};

class VirtApplianceWorker : public QObject
{
    Q_OBJECT

    public: 
        VirtApplianceWorker( const QString&, const QString& );
        ~VirtApplianceWorker();
        
    private:
        QVirt* mVirt;
        QString mVmdk;
        QString mName;
        QString which( const QString& ); 
        QString createQemuXML( const QString& );
        QString createVmwareXML( const QString& );
        
    public slots:
        void recreateDomain( const QString& );
        void startSystemQemu();
        void startSystemVmwareWS();
        void connectToInstance( const QString& );
        
    private slots:

        
    signals:
        void vnc( QUrl );
        void domainExists( QString );
        void terminated();
};

#endif
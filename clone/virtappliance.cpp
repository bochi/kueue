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

#include "virtappliance.h"
#include "settings.h"

#include <QDebug>

VirtAppliance::VirtAppliance( const QString& vmdk )
{
    qDebug() << "[VIRTAPPLIANCE] Constructing";
}

VirtAppliance::~VirtAppliance()
{
    qDebug() << "[VIRTAPPLIANCE] Destroying";
}

void VirtAppliance::setTabId( int id )
{
    mTabId = id;
}

/* 
 * 
 * VirtApplianceThread
 * 
 */

VirtApplianceThread::VirtApplianceThread( const QString& vmdk ) : QThread()
{
    mVmdk = vmdk;
    start();
}

VirtApplianceThread::~VirtApplianceThread()
{
}

void VirtApplianceThread::deleteWorker()
{
    delete mWorker;
}

void VirtApplianceThread::run()
{
    mWorker = new VirtApplianceWorker( mVmdk );
    mWorker->work();
    
    exec();
}

/*
 * 
 * VirtApplianceWorker
 * 
 */

VirtApplianceWorker::VirtApplianceWorker( const QString& vmdk ) : QObject()
{
    qDebug() << "[VIRTAPPLIANCEWORKER] Constructing" << this->thread()->currentThreadId();
    
    mVmdk = vmdk;
}

VirtApplianceWorker::~VirtApplianceWorker()
{
   qDebug() << "[VIRTAPPLIANCEWORKER] Destroying";
}

void VirtApplianceWorker::work()
{
   
}

#include "virtappliance.moc"

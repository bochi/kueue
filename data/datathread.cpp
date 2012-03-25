/*
              kueued - create xml data for kueue's qmon 
              (C) 2012 Stefan Bogner <sbogner@suse.com>

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

#include "datathread.h"

DataThread* DataThread::instance = 0;

DataThread& DataThread::thread()
{
    if ( !instance )
    {
        qDebug() << "new inst";
        instance = new DataThread;
    }
 
    return *instance; 
}

void DataThread::destroy()
{
    if ( instance )
    {
        delete instance;
    }
      
    instance = 0;
}

DataThread& DataThread::restart()
{
    instance = new DataThread;
    return *instance;
}


DataThread::DataThread( QObject *parent ) : QThread( parent )
{
    start();
}

DataThread::~DataThread()
{
    delete mData;
    qDebug() << "[DATATHREAD] Destroying";
}

void DataThread::run()
{
    createData();
    
    emit updateQueueBrowserRequested();
    
    if ( Settings::monitorEnabled() )
    {
        emit updateQmonBrowserRequested();
    }
    
    if ( Settings::statsEnabled() )
    {
        emit updateStatsBrowserRequested();
    }
    
    exec();
}

void DataThread::createData()
{
    mData = new Data();
    
    connect( mData, SIGNAL( queueDataChanged( const QString& ) ), 
             this, SIGNAL( queueDataChanged( const QString& ) ) );
    
    connect( this, SIGNAL( updateQueueBrowserRequested() ), 
             mData, SLOT( updateQueueBrowser() ) );

    connect( mData, SIGNAL( statsDataChanged( QString ) ), 
             this, SIGNAL( statsDataChanged( QString ) ) );
    
    connect( this, SIGNAL( updateStatsBrowserRequested() ), 
             mData, SLOT( updateStatsBrowser() ) );
    
    connect( mData, SIGNAL( qmonDataChanged( QString ) ), 
             this, SIGNAL( qmonDataChanged( QString ) ) );
    
    connect( this, SIGNAL( updateQmonBrowserRequested() ), 
             mData, SLOT( updateQmonBrowser() ) );
}

void DataThread::newDataSlot()
{
    delete mData;
    delete this;
}

void DataThread::updateQueueBrowserSlot()
{
    emit updateQueueBrowserRequested();
}

void DataThread::updateQmonBrowserSlot()
{
    emit updateQmonBrowserRequested();
}

void DataThread::updateStatsBrowserSlot()
{
    emit updateStatsBrowserRequested();
}

#include "datathread.moc"

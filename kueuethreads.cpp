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

#include "kueuethreads.h"
#include "nsa/nsajob.h"
#include "unity/archiveextract.h"

KueueThreads* KueueThreads::instance = 0;

KueueThreads& KueueThreads::getInstance()
{
    if ( !instance )
    {
        instance = new KueueThreads();
    }
 
    return *instance; 
}

void KueueThreads::destroy()
{
    if ( instance )
    {
        delete instance;
    }
      
    instance = 0;
}

KueueThreads::KueueThreads( QObject* parent ) 
{
    qDebug() << "[KUEUETHREADS] Constructing" << mThreadList.size();
    
    mStatusBar = &mStatusBar->getInstance();
    mCurrentThread = 0;
    
}

KueueThreads::~KueueThreads()
{
    qDebug() << "[KUEUETHREADS] Destroying";
}

void KueueThreads::enqueueThread( KueueThread* thread )
{   
    mThreadList.insert( mThreadList.size(), thread );
    qDebug() << "enqueue" << mThreadList.size();

    connect( thread, SIGNAL( threadStarted( QString, int ) ),
            this, SLOT( startThread( const QString&, int ) ) );
    
    connect( thread, SIGNAL( threadProgress( int ) ),
            this, SLOT( updateThreadProgress( int ) ) );
            
    connect( thread, SIGNAL( threadFinished( KueueThread* ) ),
            this, SLOT( endThread( KueueThread* ) ) );

    if ( mThreadList.size() == 1 )
    {
        qDebug() << "TL SIZE" << mThreadList.size() << "starting..";
        mCurrentThread = thread;
        mCurrentThread->start();
    }
}

void KueueThreads::next()
{
    if ( !mThreadList.isEmpty() )
    {
        qDebug() << "take 1s";
        mCurrentThread = mThreadList.first();
        mCurrentThread->start();
    }
}

void KueueThreads::cancelCurrentThread()
{
    mCurrentThread->exit();
    next();
}

void KueueThreads::startThread( const QString& text, int total )
{
    mStatusBar->startJobStatus( text, total );
}

void KueueThreads::updateThreadProgress( int p )
{
    mStatusBar->updateProgress( p );
}

void KueueThreads::endThread( KueueThread* thread )
{
    qDebug() << "thread finished";
    mStatusBar->resetStatusBar();
    mThreadList.removeAt( mThreadList.indexOf( thread ) );
    delete thread;
    next();
}

KueueThread::KueueThread( QObject* parent ): QThread( parent )
{
    qDebug() << "[KUEUETHREAD] Construcing" << currentThreadId();
}

KueueThread::~KueueThread()
{
    qDebug() << "[KUEUETHREAD] Destroying";
}

#include "kueuethreads.moc"

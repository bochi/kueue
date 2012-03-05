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

using namespace ThreadWeaver;

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
    qDebug() << "[KUEUETHREADS] Constructing";
    
    mWeaver = mWeaver->instance();
    mStatusBar = &mStatusBar->getInstance();
}

KueueThreads::~KueueThreads()
{
    qDebug() << "[KUEUETHREADS] Destroying";
}

void KueueThreads::enqueueJob( ThreadWeaver::Job* job )
{   
    connect( job, SIGNAL( jobStarted( QString,int ) ),
             this, SLOT( startJobStatus( QString,int ) ) );
    
    connect( job, SIGNAL( jobProgress( int ) ),
             this, SLOT( updateJobStatus( int ) ) );
    
    connect( job, SIGNAL( jobFinished( ThreadWeaver::Job* ) ), 
             this, SLOT( jobDone( ThreadWeaver::Job* ) ) );
             
    mWeaver->enqueue( job );
}

void KueueThreads::startJobStatus( const QString& text, int total )
{
    StatusBar::startJobStatus( text, total );
}

void KueueThreads::updateJobStatus( int p )
{
    StatusBar::updateProgress( p );
}

void KueueThreads::endJobStatus()
{
    StatusBar::resetStatusBar();
}

void KueueThreads::jobDone( ThreadWeaver::Job* job )
{
    endJobStatus();
    delete job;
}

#include "kueuethreads.moc"

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
#include "archivers/archiveextract.h"
#include "ui/threadprogress.h"

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
    
    mStatusBar = &mStatusBar->getInstance();
}

KueueThreads::~KueueThreads()
{
    qDebug() << "[KUEUETHREADS] Destroying";
}

void KueueThreads::enqueueThread( KueueThread* thread )
{   
    connect( thread, SIGNAL( threadStarted( QString, int ) ),
            this, SLOT( startThread( const QString&, int ) ) );
    
    connect( thread, SIGNAL( finished() ),
             this, SLOT( endThread() ) );

    thread->start();
}

void KueueThreads::startThread( const QString& text, int total )
{
    KueueThread* t = qobject_cast< KueueThread* >( sender() );
    ThreadProgress* p = new ThreadProgress( mStatusBar, text, total );
    
    addThreadWidget( p );
    
    connect( t, SIGNAL( threadProgress( int, QString ) ), 
             p, SLOT( updateProgress( int, QString ) ) );
    
    connect( t, SIGNAL( threadNewMaximum( int ) ), 
             p, SLOT( setMaximum( int ) ) );
    
    connect( t, SIGNAL( finished() ),
             p, SLOT( close() ) );
    
    connect( p, SIGNAL( closed( QWidget* ) ), 
             this, SLOT( removeThreadWidget( QWidget* ) ) );
}

void KueueThreads::addThreadWidget( QWidget* w )
{
    mStatusBar->addThreadWidget( w );
    //mThreadWidget->layout()->addWidget( w );
}

void KueueThreads::removeThreadWidget( QWidget* w )
{
    mStatusBar->removeThreadWidget( w );
}

void KueueThreads::endThread()
{
    KueueThread* t = qobject_cast< KueueThread* >( sender() );
    
    //mStatusBar->resetStatusBar();
    t->quit();
    t->wait();
    delete t;
    //next();
}

KueueThread::KueueThread( QObject* parent ) : QThread( parent )
{
}

KueueThread::~KueueThread()
{
}

#include "kueuethreads.moc"

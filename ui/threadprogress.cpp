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

#include "threadprogress.h"

#include <QDebug>
#include <QApplication>
#include <QFileDialog>
#include <QLayout>


ThreadProgress::ThreadProgress( KueueThread* thread )
{
    qDebug() << "[THREADPROGRESS] Constructing";
    
    hide();
    
    setFixedHeight( 21 );
    setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    
    QHBoxLayout* l = new QHBoxLayout( this );
    l->setContentsMargins( 0, 0, 0, 0 );
    l->setAlignment( Qt::AlignLeft | Qt::AlignCenter );
    setLayout( l );
    
    mLabel = new QLabel( this );
    mProgress = new QProgressBar( this );
    
    QFont font;
    font.setPixelSize( 12 );
    
    mLabel->setFont( font );
    mLabel->setMaximumHeight( 20 );
    
    mProgress->setMaximumHeight( 20 );
    mProgress->setMaximumWidth( 75 );
    mProgress->setFont( font );
    
    l->addWidget( mProgress, 0, Qt::AlignLeft );
    l->addWidget( mLabel );

    connect( thread, SIGNAL( threadStarted( QString, int ) ),
             this, SLOT( threadStarted( QString, int ) ) );
        
    connect( thread, SIGNAL( threadProgress( int, QString ) ), 
             this, SLOT( updateProgress( int, QString ) ) );
    
    connect( thread, SIGNAL( threadNewMaximum( int ) ), 
             this, SLOT( setMaximum( int ) ) );
    
    connect( thread, SIGNAL( threadFinished( KueueThread* ) ),
             this, SLOT( threadDone( KueueThread* ) ) );
    
    thread->start();
}

ThreadProgress::~ThreadProgress()
{
    qDebug() << "[THREADPROGRESS] Destroying";
}

void ThreadProgress::threadStarted( const QString& text, int total )
{
    show();
    mLabel->setText( text );
    mProgress->setMaximum( total );
}

void ThreadProgress::setMaximum( int max )
{
    mProgress->setMaximum( max );
}

void ThreadProgress::threadDone( KueueThread* thread )
{
    if ( thread->isRunning() )
    {
        thread->quit();
        thread->wait();
    }
    
    delete thread;
    
    emit done( this );
}

void ThreadProgress::updateProgress( int p, const QString& text )
{
    if ( !text.isNull() )
    {
        mLabel->setText( text );
    }
    
    mProgress->setValue( p );
}

#include "threadprogress.moc"

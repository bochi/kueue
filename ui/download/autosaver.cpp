/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
         This file has been copied and modified from arora
                 http://code.google.com/p/arora/
                  
          (C) 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>

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

#include "autosaver.h"

#include <QDir>
#include <QCoreApplication>
#include <QMetaObject>
#include <QDebug>

#define AUTOSAVE_IN  1000 * 3  // seconds
#define MAXWAIT      1000 * 15 // seconds

AutoSaver::AutoSaver( QObject *parent ) : QObject( parent )
{
    Q_ASSERT( parent );
}

AutoSaver::~AutoSaver()
{
    if ( mTimer.isActive() ) 
    {
        qDebug() << "[AUTOSAVER] still active when destroyed, changes not saved.";
        
        if ( parent() && 
             parent()->metaObject() )
        {
            qDebug() << parent() << parent()->metaObject()->className() << "should call saveIfNeccessary";
        }
    }
}

void AutoSaver::changeOccurred()
{
    if ( mFirstChange.isNull() )
    {
        mFirstChange.start();
    }

    if ( mFirstChange.elapsed() > MAXWAIT ) 
    {
        saveIfNeccessary();
    } 
    else 
    {
        mTimer.start( AUTOSAVE_IN, this );
    }
}

void AutoSaver::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == mTimer.timerId() ) 
    {
        saveIfNeccessary();
    }
    else 
    {
        QObject::timerEvent( event );
    }
}

void AutoSaver::saveIfNeccessary()
{
    if ( !mTimer.isActive() )
    {
        return;
    }
    
    mTimer.stop();
    mFirstChange = QTime();
    
    if ( !QMetaObject::invokeMethod(parent(), "save", Qt::DirectConnection) ) 
    {
        qDebug() << "[AUTOSAVER] error invoking slot save() on parent";
    }
}

#include "autosaver.moc"

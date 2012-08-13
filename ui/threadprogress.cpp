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


ThreadProgress::ThreadProgress( QObject* parent, const QString& text, int total )
{
    qDebug() << "[THREADPROGRESS] Constructing";
    
    setupUi( this ); 
    threadText->setText( text );
    threadProgress->setMaximum( total );
}

ThreadProgress::~ThreadProgress()
{
    qDebug() << "[THREADPROGRESS] Destroying";
}

void ThreadProgress::setMaximum( int max )
{
    threadProgress->setMaximum( max );
}

void ThreadProgress::updateProgress( int p, const QString& text )
{
    if ( !text.isNull() )
    {
        threadText->setText( text );
    }
    
    threadProgress->setValue( p );
}

void ThreadProgress::closeEvent( QCloseEvent* event )
{
    emit closed( this );
    QWidget::closeEvent( event );
}

#include "threadprogress.moc"

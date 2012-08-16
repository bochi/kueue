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

#include "waitwidget.h"
#include "kueue.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>

WaitWidget::WaitWidget( QObject* parent )
{
    qDebug() << "[WAITWIDGET] Constructing";
   
    QGridLayout* layout = new QGridLayout(this);
    
    mProgress = new QProgressIndicator( this, QSize( 160, 160 ) );
    mProgress->setAnimationDelay( 90 );
    mProgress->stopAnimation();
    mProgress->show();
    
    mLabel = new QLabel( this );
    
    layout->addWidget(mProgress, 2, 2, Qt::AlignCenter);
    layout->addWidget( mLabel, 3, 2, Qt::AlignCenter );
    
    layout->setRowStretch(0,0);
    layout->setRowStretch(1,10);
    layout->setRowStretch(2,5);
    layout->setRowStretch(3,10);
    layout->setRowStretch(4,0);
        
    hide();
}

WaitWidget::~WaitWidget()
{
    qDebug() << "[WAITWIDGET] Destroying";
}

void WaitWidget::activate( const QString& text )
{
    if ( text == QString::Null() )
    {
        mLabel->setText( "" );
    }
    else
    {
        mLabel->setText( "<font size='+1'><b><i>" + text + "</i><b></font>" );
    }
    
    mProgress->startAnimation();
    show();
}

void WaitWidget::deactivate()
{
    mProgress->stopAnimation();
    hide();
}

#include "waitwidget.moc"

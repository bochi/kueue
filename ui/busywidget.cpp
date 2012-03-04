/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>

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

#include "busywidget.h"
#include "kueue.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>

BusyWidget::BusyWidget( QObject* parent )
{
    qDebug() << "[BUSYWIDGET] Constructing";

    setAttribute( Qt::WA_TranslucentBackground, true );
    
    QHBoxLayout* hLayout = new QHBoxLayout( this );

    mProgress = new QProgressIndicator( this, QSize( 160, 160 ) );
    mProgress->setAnimationDelay( 90 );
    mProgress->stopAnimation();
    mProgress->show();
    
    hLayout->addStretch();
    hLayout->addWidget( mProgress );
    hLayout->addStretch();
    
    hide();
}

BusyWidget::~BusyWidget()
{
    qDebug() << "[BUSYWIDGET] Destroying";
}

void BusyWidget::paintEvent(QPaintEvent* event)
{
    QColor backgroundColor = palette().dark().color();
    backgroundColor.setAlpha( 70 );
    QPainter customPainter( this );
    customPainter.fillRect( rect(), backgroundColor );
}

void BusyWidget::activate()
{
    mProgress->startAnimation();
    show();
}

void BusyWidget::deactivate()
{
    mProgress->stopAnimation();
    hide();
}


#include "busywidget.moc"

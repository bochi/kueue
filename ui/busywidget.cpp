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

#include "busywidget.h"
#include "kueue.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>

BusyWidget::BusyWidget( QObject* parent )
{
    qDebug() << "[BUSYWIDGET] Constructing";

    setAttribute( Qt::WA_TranslucentBackground, true );
    
    QGridLayout* layout = new QGridLayout(this);
    
    mProgress = new QProgressIndicator( this, QSize( 160, 160 ) );
    mProgress->setAnimationDelay( 90 );
    mProgress->stopAnimation();
    mProgress->show();
    
    mLabel = new QLabel( this );
    mButton = new QPushButton( this );
    mButton->setStyleSheet( "background-color: rgba( 255, 255, 255, 0% );" );
    
    connect( mButton, SIGNAL( pressed() ),
             this, SLOT( deactivate() ) );

    mButton->setIcon( QIcon(":/icons/menus/quit.png"));
    
    layout->addWidget(mLabel, 0, 2, Qt::AlignRight );
    layout->addWidget(mProgress, 2, 2, Qt::AlignCenter);
    layout->addWidget(mButton, 4, 2, Qt::AlignRight );
    
    layout->setRowStretch(0,0);
    layout->setRowStretch(1,10);
    layout->setRowStretch(2,5);
    layout->setRowStretch(3,10);
    layout->setRowStretch(4,0);
        
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

void BusyWidget::activate( const QString& text )
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

void BusyWidget::deactivate()
{
    mProgress->stopAnimation();
    hide();
}


#include "busywidget.moc"

/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
          
                   This code was taken from krdc
         Copyright (C) 2007-2012 Urs Wolfer <uwolfer@kde.org>
          
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

#include "vncwidget.h"

#include <QPushButton>
#include <QEvent>

VncWidget::VncWidget( RemoteView::Quality quality )
{
    qDebug() << "[VNCWIDGET] Constructing";

    mQuality = quality;
    mVncView = 0;
    
    mLayout = new QGridLayout( this );
    setLayout( mLayout );

    mDlButton = new QPushButton( this );
    mDlButton->setText( "Download appliance..." );
    
    mLabel = new QLabel( this );
    mLabel->setText( "Test" );

    mOverlayLayout = new QStackedLayout();
    mOverlayLayout->setStackingMode(QStackedLayout::StackAll);
    
    mSubLayout = new QGridLayout();
        
    mWaitWidget = new WaitWidget( this );
    mOverlayLayout->addWidget( mWaitWidget );

    mLayout->addLayout( mOverlayLayout, 2, 0, Qt::AlignCenter );
    
    connect( mDlButton, SIGNAL( clicked( bool ) ),
             this, SIGNAL( downloadRequested() ) );

    show();
}

VncWidget::~VncWidget()
{
    qDebug() << "[VNCWIDGET] Destroying";
}

void VncWidget::createVncView( const QUrl& url )
{
    if ( mVncView != 0 )
    {
        mOverlayLayout->removeWidget( mVncView );
        mVncView->startQuitting();
        delete mVncView;
        mVncView = 0;
    }
    
    mWidget = new QWidget( this );
    mVncView = new VncView( mWidget, url, mQuality );
    mSubLayout->addWidget( mVncView, 0, 0, Qt::AlignCenter );
    
    mWidget->setLayout( mSubLayout );
        
    connect( mVncView, SIGNAL( somethingWentWrong() ), 
             this, SIGNAL( somethingWentWrong() ) );
    
    connect( mVncView, SIGNAL( repainted() ),
             mWaitWidget, SLOT( deactivate() ) );
    
    mOverlayLayout->addWidget( mWidget );
    
    mLayout->addWidget( mLabel, 0, 0, Qt::AlignLeft );
    mLayout->addWidget( mDlButton, 4, 0, Qt::AlignRight );
    
    mLayout->setRowStretch(0,0);
    mLayout->setRowStretch(1,10);
    mLayout->setRowStretch(2,0);
    mLayout->setRowStretch(3,10);
    mLayout->setRowStretch(4,0);
    
    mWaitWidget->activate( "Waiting for VNC connection..." );
    mVncView->show();
    mVncView->start();
}

void VncWidget::getFocus()
{
    if ( mVncView != 0 )
    {
        mVncView->setFocus();
    }
}

bool VncWidget::event( QEvent* e )
{
    if ( e->type() == QEvent::Enter )
    {
        getFocus();
    }
    
    QWidget::event( e );
}

void VncWidget::wait( const QString& text )
{
    mWaitWidget->activate( text );
}

void VncWidget::stopWaiting()
{
    mWaitWidget->deactivate();
}

#include "vncwidget.moc"

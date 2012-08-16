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

#ifndef VNCWIDGET_H
#define VNCWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QLabel>

#include "vnc/vncview.h"
#include "waitwidget.h"

class VncWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit VncWidget( QObject* parent = 0L, RemoteView::Quality = RemoteView::High );
        ~VncWidget();
        
        VncView* vnc() { return mVncView; }
        int tabId() { return mTabId; }
        
    private:
        VncView* mVncView;
        QWidget* mWidget;
        QUrl mUrl;
        int mTabId;
        QGridLayout* mLayout;
        QGridLayout* mSubLayout;
        QStackedLayout* mOverlayLayout;
        WaitWidget* mWaitWidget;
        QPushButton* mDlButton;
        QLabel* mLabel;
        RemoteView::Quality mQuality;
        
    protected:
        bool event( QEvent* );
        
    public slots:
        void setTabId( int );    
        void createVncView( const QUrl& );
        void getFocus();
        void closeWidget();
        
    signals:
        void somethingWentWrong();
        void widgetClosed( int );
        void downloadRequested();
};

#endif
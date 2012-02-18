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

#ifndef UNITYBROWSER_H
#define UNITYBROWSER_H

#include "network.h"
#include "ui/kueuewindow.h"
#include "unity/unitypage.h"
#include "ui/browsers/search/webviewsearch.h"

#include <QWebView>
#include <QWebFrame>
#include <QDialog>
#include <QMenu>
#include <QToolBar>

class UnityBrowser : public QWebView
{
    Q_OBJECT
     
    public:
        explicit UnityBrowser( QWidget* parent = 0L );
        ~UnityBrowser();
        
        QString currentSR();
          
    public slots:
        void querySR( const QString& );
        void historyBack();
        QWebPage* newWindow();
        
    private slots:
        void mousePressEvent( QMouseEvent* );
        void urlHovered( const QString&, const QString& = 0, const QString& = 0 );
        void contextMenu( QMouseEvent*, const QString& );
        void contextMenuEvent( QContextMenuEvent * ev );
        void openFileBrowser();
        void openSearch();
        void fillOutProduct();
        
        void openWebEditor();
        void copyToClipboard();
        void goBackToSr();
        void saveImage();
        void openWebInspector();

    private:
        UnityPage* mUnityPage;
        QMenu* productMenu( QMenu* parent );
        QUrl mUrl;
        bool isTextArea( QWebElement );     
        bool isProductField( QWebElement );
        
    signals:
        void searchRequested( QString );
        void currentSrChanged( QString );
};

class UnityWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit UnityWidget( QObject* parent = 0L );
        ~UnityWidget();
        
        UnityBrowser* browser() { return mUnityBrowser; }
        int tabId() { return mTabId; }
        
    private:
        UnityBrowser* mUnityBrowser;
        QToolBar* mToolBar;
        QToolButton* mSrButton;
        int mTabId;
        
    public slots:
        void currentSrChanged( QString );
        void setTabId( int );
};

#endif

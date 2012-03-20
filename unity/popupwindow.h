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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QWebView>
#include <QDialog>
#include <QMouseEvent>

class PopupWindowWebView : public QWebView
{
    Q_OBJECT
    
    public:
        explicit PopupWindowWebView( QWidget* parent = 0L );
        ~PopupWindowWebView();
        
    public slots:
        QWebElement getFocusedElement( const QPoint& pos );
        QWebPage* newWindow();
        
    private slots:
        void contextMenu( QMouseEvent*, const QString& );
        void mousePressEvent( QMouseEvent* );
        void contextMenuEvent(QContextMenuEvent* );
        void contextMenuItemTriggered( QAction* ); 
                  
    private:
        QUrl mUrl;
        bool isTextArea( QWebElement );
};

class PopupWindow : public QDialog
{
    Q_OBJECT
     
    public:
        explicit PopupWindow( QNetworkAccessManager*, QWidget* parent = 0L );
        ~PopupWindow();
        
        QWebPage* webPage() { return mWebView->page(); }
        
    private:
        PopupWindowWebView* mWebView;
        QWidget* mWidget;
        QUrl mUrl;
        
    public slots:
        void resizeRequested( const QRect& );
        void contentChanged();
        void loadFinished();
        void closeWindow();

};

class PopupWindowWebPage : public QWebPage
{
    Q_OBJECT
    
    public:
        explicit PopupWindowWebPage( QObject* parent = 0L );
        ~PopupWindowWebPage();
        
    private:
        int mEmailStage;
        QString okJS;
        QString mCurrentSR;
        QWebPage* createWindow( QWebPage::WebWindowType type );
        StatusBar* mStatusBar;
        
    public slots:
        void handleUnsupportedContent( QNetworkReply* );
        void download( const QNetworkRequest& req );
        
    private slots:
        void pageLoaded();
};



#endif
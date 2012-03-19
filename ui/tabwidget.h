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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "ui/browsers/qmonbrowser.h"
#include "ui/browsers/unitybrowser.h"
#include "ui/browsers/statsbrowser.h"
#include "ui/browsers/queuebrowser.h"
#include "ui/kueuewindow.h"
#include "ui/download/downloadmanager.h"
#include "ui/statusbar.h"
#include "ui/browsers/search/webviewsearch.h"

#include <QTabWidget>
#include <QToolButton>

class DownloadManager;
class KueueWindow;
class TabButton;
class TabBar;

class TabWidget : public QTabWidget
{
    Q_OBJECT

    public:
        static TabWidget& tw();
        static void destroy();
        
        static void openInUnity( const QString& id )
        {
            TabWidget::tw().openInUnityImp( id );
        }
        
        static void newUnityWithSR( const QString& id )
        {
            TabWidget::tw().addUnityBrowser( id );
        }
        
    private:
        static TabWidget* instance;
        TabWidget(QWidget *parent = 0L);
        ~TabWidget();
        
        TabBar* mBar;
        TabButton* mMenuButton;
        QWidget* mPersonalTab;
        QWidget* mMonitorTab;
        QWidget* mStatsTab;
        UnityWidget* mUnityTab;
        QueueBrowser* mQueueBrowser;
        QMonBrowser* mQmonBrowser;
        StatsBrowser* mStatsBrowser;
        UnityBrowser* mUnityBrowser;
        KueueWindow* mKueueWindow;
        QList<UnityWidget*> mUnityWidgetList;
        QMap<int, UnityBrowser*> mUnityBrowserMap; 
        StatusBar* mStatusBar;
        WebViewSearch* webViewSearch( int );
        QMenu* kueueMainMenu();
        QAction* mActionQuit;
        QAction* mActionConfig;
        QAction* mActionShowSR;
        QAction* mActionShowCR;
        QAction* mActionTodoShowUp;
        QAction* mActionTodoShowStat;
        QAction* mActionTodoShowSmilies;
        QAction* mActionSortUpdate;
        QAction* mActionSortAge;
        QAction* mActionAwaitingCustomer;
        QAction* mActionAwaitingSupport;
        QAction* mActionOthers;
        QAction* mActionExpandSrTables;
        QAction* mActionCloseSrTables;
        QAction* mActionAbout;
        QAction* mActionBug;
        QAction* mActionFeature;
        QAction* mActionHelp;
        QAction* mActionWebsite;
        QAction* mActionAddUnityTab;
        QAction* mActionNSA;
        
    public slots:
        void setTabsPosition();
        void addUnityBrowser( const QString& = QString::Null() );
        void removeUnityBrowser( int );
        void showMonitorTab( bool );
        void showStatsTab( bool );
        void showPersonalTab( bool );
        void showUnityTab( bool );
        void showSrInUnityBrowser( int, const QString& );
        void switchToMainUnityTab();
        void switchToTab( int );
        void updateUiData();
        void openClipboardInUnity( int );
        void openInUnityImp( const QString& );
        void updateQueueBrowser( const QString& = "bla" );
        void updateQmonBrowser();
        void updateStatsBrowser();
        void refreshTabs();
        
    private slots:
        void tabMiddleClicked( int, QPoint );
        void tabRightClicked( int, QPoint );
        void setMenus();
        void unityTabMenu( int tab, const QPoint& p );
        void closeActionTriggered();
        void clipboardActionTriggered();
        void setShowSR( bool );
        void setShowCR( bool );
        void setTodoShowUp( bool );
        void setTodoShowStat( bool );
        void setTodoShowSmilies( bool );
        void setSortAge( bool );
        void setSortUpdate( bool );
        void setShowAwaitingCustomer( bool );
        void setShowAwaitingSupport( bool );
        void setShowStatusOthers( bool );
        void expandAllTables();
        void closeAllTables();
        void aboutDialog();
        void reportBug();
        void suggestFeature();
        void openHelp();
        void helpClosed();
        void openWebsite();
        void showSearch();
        void makeNsaReport();
        
    signals:
        void unityTabMiddleClicked( int );
        void openConfig();

};

class TabBar : public QTabBar
{
    Q_OBJECT

    public:
        explicit TabBar( QObject* parent = 0L );
        ~TabBar();
        
        int selectTab( const QPoint& ) const;
        
    private:
        void mousePressEvent ( QMouseEvent * event );

    signals:
        void tabRightClicked( int, QPoint );
        void tabMiddleClicked( int, QPoint );
};

class TabButton : public QToolButton
{
    Q_OBJECT
    
    public: 
        explicit TabButton( QWidget* parent = 0L );
        ~TabButton();
     
    protected:
        void mousePressEvent( QMouseEvent* );
        
    signals:
        void middleClicked();
};

#endif

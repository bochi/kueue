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

#ifndef UNITYPAGE_H
#define UNITYPAGE_H

#include "unity/unitynetwork.h"
#include "ui/statusbar.h"

#include <QWebPage>
#include <QWebElement>

class UnityPage : public QWebPage
{
    Q_OBJECT
     
    public:
        explicit UnityPage( QObject* parent = 0L );
        ~UnityPage();
        
        bool navReady() { return mNavReady; }
        bool loggedIn() { return mLoggedIn; }
        bool isNsaReport() { return mIsNsaReport; }
        QWebHitTestResult getElementAt( const QPoint& );
        QString currentSr() { return mCurrentSR; }
        QWebFrame* viewFrame() { return mViewFrame; }

    public slots:
        void querySR( const QString& );
        void setElementText( QWebElement, const QString& );
        void sendEmail();
        void setSS( const QString& = "NONE" );
        void setSC( const QString& = "NONE" );
        void logout();
        void saveNsaReport();
        void handleUnsupportedContent(QNetworkReply*);
        void fillOutProduct( const QString&, const QString& );
        void saveCurrentSR();
        
    private slots:
        void pageLoaded();
        void loginToUnity();
        void loggedOut();
        void passwordReceived();
        void antiIdle();
        void addFrame( QWebFrame* );
        void fixQueryBox();
        void goHome();
        void goHomeFirst();
        void doQuery();
        void goToActivities();
        void newActivity();
        void setSSconfirmed( const QString&, const QString& );
        void setSSfirst();
        void setSSsecond();
        void setSSthird();
        void unsetJsConfirm();
        void getServiceJS();
        void getCurrentSR();
        void selectionToClipboard();
        void download( const QNetworkRequest& );
        void fillOutProductNext();
        void saveCurrentActivity();
        void setScConfirmed( const QDateTime&, const QString& );
        void setScFirst();
        void setScSecond();
        void setScThird();
        void setScFourth();
        void setScFifth();
        void setScSixth();

        
    private:
        QWebPage* createWindow( QWebPage::WebWindowType type );
        QWebPage* mNewPage;
        
        QWebFrame* mViewFrame;
        QWebFrame* mBarFrame;
        QWebFrame* mMenuFrame;
        QWebFrame* mViewBarFrame;
        
        QWebElement mProdElement;
        
        StatusBar* mStatusBar;
        
        QString mQuerySR;
        QString mServiceJS;
        QString mLogoutJS;
        QString mNewJS();
        QString mCurrentSR;
        QString mNewStatus;
        QString mFileName;
        QString mProduct;
        QString mSaveJS;
        QString mSsReason;
        
        QDateTime mScDateTime;
        
        QTimer* mTimer;

        UnityNetwork* mNAM;
        
        bool mDontLogin;
        bool mNavReady;
        bool mLoggedIn;
        bool mIsNsaReport;
        bool mSetSS;
        bool mSetSC;
        bool mNoJsConfirm;
        
    protected:
        void javaScriptAlert( QWebFrame*, const QString& );
        bool javaScriptConfirm( QWebFrame*, const QString& );

    signals:
        void contentChanged();
        void currentSrChanged( QString );
};

#endif

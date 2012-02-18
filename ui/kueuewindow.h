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

#ifndef KUEUEWINDOW_H
#define KUEUEWINDOW_H

#include "ui/browsers/unitybrowser.h"
#include "data/database.h"
#include "ui/detailwindow.h"
#include "ui/browsers/qmonbrowser.h"
#include "ui/browsers/queuebrowser.h"
#include "ui/browsers/statsbrowser.h"
#include "ui/tabwidget.h"
#include "configdialog.h"
#include "config.h"
#include "ui/systray.h"

#include <QMainWindow>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMovie>
#include <QMenu>

#ifdef USE_PHONON
#include <phonon/MediaObject>
#endif

namespace Ui {
    class KueueWindow;
}

class UnityPage;
class UnityBrowser;

class KueueWindow : public QMainWindow
{
    Q_OBJECT
    
    private:
        static KueueWindow* instance;
        KueueWindow( QWidget *parent = 0L );
        ~KueueWindow();
        
        Ui::KueueWindow* ui;
        QGridLayout* mNotificationLayout;
        
        #ifdef USE_PHONON
            Phonon::MediaObject* mMusic;
        #endif
            
    public:
        static KueueWindow& win();
        static void destroy();
        
        void playSound( const QString& file );
        void showDetailedDescription( QString = "", bool = false );
        void showDetailedDescriptionNB( QString = "" );
        
    public slots:
        void showWindow();
        void hideWindow();
        void toggleWindow();

    private slots:
        void closeEvent( QCloseEvent* );
        void detailWindowClosed( DetailWindow* );
        
    protected:
        bool eventFilter( QObject*, QEvent* );
};

#endif // KUEUEWINDOW_H

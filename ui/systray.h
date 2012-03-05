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

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QSystemTrayIcon>
#include <QMovie>
#include <QAction>

class Systray : public QSystemTrayIcon
{
    Q_OBJECT

    private:
        static Systray* instance;
        Systray();
        Systray( const Systray& ) {}
        ~Systray();

        QString dbusInterfaceName;
        QString dbusServiceName;
        QString dbusPath;
        QMovie* mMovie;
        QString mCurrentMsg;
        bool mIsGnome;
        
    public:
        static Systray& tray();
        static void destroy();
        
    public slots:
        void notify( const QString&, QString, QString, const QString& );
        void requiresAttention( const bool& );
    
    private slots:
        void trayIconActivated( QSystemTrayIcon::ActivationReason );
        void trayMenuItemTriggered( QAction* );
        void notificationActionInvoked( uint, QString );
        void closeNotification( uint );
        void changeIcon();
        void writeToFile( const QString& );
        void winMessageClicked();
};

#endif

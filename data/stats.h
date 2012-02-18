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

#ifndef STATS_H
#define STATS_H

#include "settings.h"

#include <QWebView>
#include <QContextMenuEvent>

class Stats : public QObject
{
    Q_OBJECT

    public: 
        Stats();
        ~Stats();
        
    private:
        QTimer* mTimer;
        QString getWF( const QString& );
        QNetworkReply* mTts;
        QNetworkReply* mCsat;

    public slots:
        void update();
        void updatePersonal();
        void updateTeam();

    private slots:
        void csatJobDone();
        void ttsJobDone();
        
    signals:
        void statsChanged();
        void initialUpdate( int, int );
        void initialUpdateProgress( int );
        void initialUpdateDone();
};     

class CsatItem
{
    public:
        QString engineer;
        QString wfid;
        QString sr;
        QString customer; 
        QString bdesc;
        QString engsat;
        QString srsat;
        bool rts;
};

class TtsItem
{
    public:
        QString engineer;
        QString sr;
        QString bdesc;
        QString customer;
        int tts;
};

class ClosedItem
{
    public:
        QString sr;
        int days;
};

#endif

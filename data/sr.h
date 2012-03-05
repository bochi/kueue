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

#ifndef _SR_H_
#define _SR_H_

#include <QDateTime>
#include <QStringList>

class SR : public QObject
{
    Q_OBJECT

    public: 
        SR( const QString& );
        ~SR();
        
        /* Solution Suggested set? */
        bool ss() { return mSs; }
        
        /* Is it a CR? */
        bool isCR() { return mIsCR; }

        /* Returns the SR age in days */
        int age() { return mAge; }
        
        /* Returns the last update in days */
        int lastUpdateDays() { return mLastUpdateDays; }
        
        /* Returns the opened date as QDateTime */
        QDateTime opened() { return mOpened; }
        
        /* Returns the last update date as QDateTime */
        QDateTime lastUpdate() { return mLastUpdate; }
        
        /* Returns the SR number */
        QString id() { return mId; }
        
        /* Returns the customer */
        QString customer() { return mCustomer; }
        
        /* Returns the contact (ie. customer name) */
        QString contact() { return mContact; }
        
        /* Returns the brief description */
        QString briefDesc() { return mBriefDesc; }
        
        /* Returns the status */
        QString status() { return mStatus; }
        
        /* Returns the detailed description */
        QString detailedDesc() { return mDetailedDesc; }
        
        /* Returns the todolist */
        QStringList todoList() { return mTodoList; }

	/* Returns the display style */
	QString display() { return mDisplay; };
  
    private: 
        bool mSs;
        bool mIsCR;
        int mAge;
        int mLastUpdateDays;
        QDateTime mOpened;
        QDateTime mLastUpdate;
        QString mSrdata;
        QString mId;
        QString mCustomer;
        QString mContact;
        QString mBriefDesc;
        QString mStatus;
        QString mDetailedDesc;
        QString mDisplay;
	QStringList mTodoList;

    public slots:
        void setSs( bool );
        void setIsCR( bool );
        void setAge( int );
        void setLastUpdateDays( int );
        void setOpened( const QDateTime& );
        void setLastUpdate( const QDateTime& );
        void setId( const QString& );
        void setCustomer( const QString& );
        void setContact( const QString& );
        void setBriefDesc( const QString& );
        void setStatus( const QString& );
        void setDetailedDesc( const QString& );
	void setDisplay( const QString& );
        void makeTodoList();
        
};

#endif

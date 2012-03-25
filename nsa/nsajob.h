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

#ifndef NSAJOB_H_
#define NSAJOB_H_

#include "kueuethreads.h"


class NSATableItem;

class NSAJob : public KueueThread
{
    Q_OBJECT

    public: 
        NSAJob( const QString&, const QString& );
        ~NSAJob();
        
    private:
        QDir mNsaDir;
        QString mSupportConfig;
        QString mSupportConfigFile;
        QString mCritical;
        QString mWarning;
        QString mRecommended;
        
    protected: 
        void run();
        
    signals:
        void startedReport( const QString&, int );
        void finishedReport( const QString& );
};

class NSATableItem
{
    public:
        enum type
        {
            Recommended = 1,
            Warning = 3,
            Critical = 4
        };
        
        int type;
        QString category;
        QString desc;
        QList<QStringList> linkList;
};

class NSASummaryItem
{
    public:
        QString archive;
        QString scriptversion;
        QDate scriptdate;
        QDateTime rundate;
        QString hostname;
        QString arch;
        QString kernel;
        QString slesversion;
        QString slessp;
        QString oesversion;
        QString oessp;
};

#endif

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

#ifndef THREADPROGRESS_H
#define THREADPROGRESS_H
 
#include "kueuethreads.h"
 
#include <QFrame>
#include <QLabel>
#include <QProgressBar>


class ThreadProgress : public QFrame
{
    Q_OBJECT

    public:
        ThreadProgress( KueueThread* );
         ~ThreadProgress();
         
    private:
        QLabel* mLabel;
        QProgressBar* mProgress;
        
    public slots:    
        void threadStarted( const QString&, int );
        void setMaximum( int );
        void updateProgress( int, const QString& );
        void threadDone();
        
    signals:
        void done( QWidget* );
};
 
#endif

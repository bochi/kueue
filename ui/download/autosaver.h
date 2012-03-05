/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
         This file has been copied and modified from arora
                 http://code.google.com/p/arora/
                  
          (C) 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>

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


#ifndef AUTOSAVER_H
#define AUTOSAVER_H

#include <QObject>
#include <QBasicTimer>
#include <QDateTime>

/*
    This class will call the save() slot on the parent object when the parent changes.
    It will wait several seconds after changed() to combining multiple changes and
    prevent continuous writing to disk.
  */

class AutoSaver : public QObject
{
    Q_OBJECT

    public:
        AutoSaver( QObject *parent );
        ~AutoSaver();
        void saveIfNeccessary();

    public slots:
        void changeOccurred();

    protected:
        void timerEvent( QTimerEvent *event );

    private:
        QBasicTimer mTimer;
        QTime mFirstChange;
};

#endif // AUTOSAVER_H


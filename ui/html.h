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

#ifndef _HTML_H_
#define _HTML_H_

#include "data/sr.h"
#include "data/stats.h"
#include "data/qmon.h"
#include "nsa/nsajob.h"

class SR;
class NSATableItem;

class HTML
{
    public:
        static QString styleSheet();
        static QString pageHeader( const QString&, int );
        static QString SRTable( SR* );
        static QString pageFooter();
        static QString qmonPageHeader();
        static QString qmonTableHeader( const QString& );
    //    static QString qmonSrInQueue( SiebelItem* );
        static QString qmonTableFooter();
//        static QString csatTable( CsatItem* );
        static QString csatTableHeader( int, int, int );
        static QString closedTableHeader( int, int );
  //      static QString closedTable( TtsItem* );
        static QString statsPageHeader();
        
        static QString nsaPageHeader( NSASummaryItem );
        static QString nsaTableStart( const QString& );
        static QString nsaTableItem( NSATableItem );
        static QString nsaTableEnd();
        static QString nsaPageEnd();
        
    private:
        static QString timeString( int );
        static int secDays( int );
        static QString csatDate();
    
};


#endif

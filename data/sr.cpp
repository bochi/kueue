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

#include "sr.h"
#include "settings.h"


SR::SR( const QString& id )
{
    mId = id;
}

SR::~SR()
{
}

void SR::makeTodoList()
{
    if ( Settings::todoShowStat() )
    {
        if ( mStatus == "Assigned" || mStatus == "Unassigned" ) 
        {
            mTodoList.append( "[2]The SR is in status " + mStatus + ", that's probably incorrect. Please check and change status if needed." );
        }
    }
  
    if ( Settings::todoShowUp() )
    {
        if ( mStatus == "Awaiting Novell Support" && mLastUpdateDays >= 3 ) 
        {
            mTodoList.append( "[1]There was no activity for " + QString::number( mLastUpdateDays ) + " days and the customer is waiting for Novell, please update SR asap!" );
        }
        else if ( mStatus != "Suspended" && mLastUpdateDays >= 5 ) 
        {    
            mTodoList.append( "[1]There was no activity for " + QString::number( mLastUpdateDays ) + " days, please update SR asap!" );
        }
    }
  
    if ( mTodoList.isEmpty() ) 
    {
        mTodoList.append( "[3]Nothing." ); 
    }
}

void SR::setSs( bool s )
{
    mSs = s;
}

void SR::setIsCR( bool c )
{
    mIsCR = c;
}

void SR::setAge( int a )
{
    mAge = a;
}

void SR::setLastUpdateDays( int l )
{
    mLastUpdateDays = l;
}

void SR::setOpened( const QDateTime& o )
{
    mOpened = o;
}

void SR::setLastUpdate( const QDateTime& l )
{
    mLastUpdate = l;
}

void SR::setId( const QString& i )
{
    mId = i;
}

void SR::setCustomer( const QString& c )
{
    mCustomer = c;
}

void SR::setContact( const QString& c )
{
    mContact = c;
}

void SR::setBriefDesc( const QString& b )
{
    mBriefDesc = b;
}

void SR::setStatus( const QString& s )
{
    mStatus = s;
}

void SR::setDetailedDesc( const QString& d )
{
    mDetailedDesc = d;
}

void SR::setDisplay( const QString& d )
{
    mDisplay = d;
}

#include "sr.moc"

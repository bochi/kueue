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
    
    PLEASE NOTE:
    
    This is all very ugly and hacky, mostly due to the fact that there is no
    API and this has to be workarounded by parsing urls, titles, elements etc.
    
    If somebody knows a better way to achieve similar functionality, I'm
    open for suggestions :-)
*/

#include "unitypage.h"

// Set "Schedule for close" activity and change status accordingly

void UnityPage::setSC( const QString& sr )
{
    if ( sr == "NONE" )
    {
        mCalendarDialog = new CalendarDialog( this, mCurrentSR );
    }
    else
    {
        mCalendarDialog = new CalendarDialog( this, sr );
    }
    
    connect( mCalendarDialog, SIGNAL( accepted() ),
             this, SLOT( setScConfirmed() ) );
    
    connect( mCalendarDialog, SIGNAL( rejected() ), 
             this, SLOT( setScRejected() ) );
    
    mCalendarDialog->exec();
}

// If confirmed, we first navigate to the SR

void UnityPage::setScConfirmed()
{
    emit pageErbert( "Setting SR#" + mCalendarDialog->sr() + " to Schedule For Close" );
 
    if ( ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Activities" ) &&
         ( mCurrentSR == mCalendarDialog->sr() ) )
    {
        mSetSC = true;
        newActivity();
    }
    else
    {
        mSetSC = true;
        querySR( mCalendarDialog->sr() );
    }
}

void UnityPage::setScRejected()
{
    delete mCalendarDialog;
}

// Then we set the activity audience to "Public"

void UnityPage::setScFirst()
{
    mNoJsConfirm = true;
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScSecond() ) );

    QString changeJS;

    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QWebElement done;
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at( i ).attribute( "id" ).contains( "s_2_2_20" ) )
        {
            changeJS = c.at( i ).attribute( "onchange" );
            
            QWebElementCollection d = c.at( i ).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute( "selected");
                
                if ( d.at( i ).attribute( "value" ) == "Public" )
                {
                    e = d.at( i );
                }
            }

            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
        
        else if ( c.at( i ).attribute( "id" ).contains( "s_2_2_32" ) )
        {
            changeJS = c.at( i ).attribute( "onchange" );
            
            QWebElementCollection d = c.at( i ).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute("selected");
                
                if ( d.at( i ).attribute( "value" ) == "Schedule For Close" )
                {
                    e = d.at( i );
                }
            }
        
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }
    
    mSetSC = false;
}

// Next we set the Status to "In Progress"

void UnityPage::setScSecond()
{
    mSetSC = true; 
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScThird() ) );

    QString changeJS;
    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at(i).attribute( "id" ).contains( "s_2_2_35" ) )
        {
            changeJS = c.at( i ).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute( "selected" );
                
                if ( d.at( i ).attribute( "value" ) == "In Progress" )
                {
                    e = d.at( i );
                }
            }
            
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }
    
    mSetSC = false; 
}

// Then we set the date according to the picked date

void UnityPage::setScThird()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    mSetSC = true;
    QString changeJS;
    
    disconnect( mViewFrame, 0, 0, 0 );

    QWebElementCollection rc = mViewFrame->findAllElements( "input" );
    
    for ( int i = 0; i < rc.count(); ++i ) 
    {  
        if ( ( rc.at( i ).attribute( "id" ).contains( "s_2_2" ) ) && 
             ( rc.at( i ).attribute( "tabindex" ).contains( "2008" ) ) )
        {
            changeJS = rc.at( i ).attribute( "onchange" );
            rc.at( i ).setAttribute( "value", mCalendarDialog->dateTime().toString( "M/d/yyyy hh:mm:ss AP" ) );
        }
    }
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScFourth() ) );
    
    mViewFrame->evaluateJavaScript( changeJS );
    
    mSetSC = false;
}
    
// ...and finally we save the activity

void UnityPage::setScFourth()
{
    mSetSC = true;
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ),
             this, SLOT( setScFifth() ) );

    QString id;
    QString saveJS;
    
    if ( mIsCr )
    {
        id = "s_5_1_14";
    }
    else
    {
        id = "s_2_1_14";
    }
    
    QWebElementCollection sc = mViewFrame->findAllElements( "a" );
    
    for ( int i = 0; i < sc.count(); ++i ) 
    {  
        if ( sc.at( i ).attribute( "id" ).contains( id ) )
        {
            saveJS = sc.at( i ).attribute( "href" ).remove( "JavaScript:" );
        }
    }
       
    if ( mIsCr )
    {
        mViewFrame->evaluateJavaScript( "top._swescript.HandleAppletClickSI('SWEApplet1')" );
    }
    
    mViewFrame->evaluateJavaScript( saveJS );
    
    mSetSC = false;
}

// Lastly we set the status to "Schedule for close"...

void UnityPage::setScFifth()
{
    mSetSC = true;
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScSixth() ) );
    
    QWebElementCollection sc = mViewFrame->findAllElements( "select" );
    QString changeJS;
    QWebElement e;
    
    for ( int i = 0; i < sc.count(); ++i ) 
    {  
        if ( ( sc.at( i ).attribute( "id" ).contains( "s_1_1" ) ) &&
             ( sc.at( i ).attribute( "tabindex" ) == "1005" ) )
        {
            changeJS = sc.at( i ).attribute( "onchange" );
            
            QWebElementCollection d = sc.at( i ).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute( "selected" );
                
                if ( d.at( i ).attribute( "value" ) == "Schedule For Close" )
                {
                    e = d.at( i );
                }
            }
            
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }
    
    mSetSC = false;
}

// ...and save the SR. Done :-)

void UnityPage::setScSixth()
{
    mSetSC = true;
    mSaveSr = true;
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( actionDone() ) );
    
    delete mCalendarDialog;
    mSetSC = false;
}

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
#include "ui/calendardialog.h"

void UnityPage::setSC( const QString& sr )
{
    CalendarDialog* cd = new CalendarDialog( this, sr );
    cd->setWindowTitle( "Schedule for close for SR#" + sr );
    
    connect( cd, SIGNAL( datePicked( QDateTime, QString ) ),
             this, SLOT( setScConfirmed( QDateTime, QString ) ) );
    
    cd->exec();
    delete cd;
}

void UnityPage::setScConfirmed( const QDateTime& dt, const QString& sr )
{
    disconnectShortcuts();
    mSetSC = true;
    mScDateTime = dt;
    
    if ( sr == "NONE" )
    {
        querySR( mCurrentSR );
    }
    else
    {
        querySR( sr );
    }
}

void UnityPage::setScFirst()
{
    mNoJsConfirm = true;
    
    disconnect( mViewFrame, 0, 0, 0 );

    QString changeJS;

    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QWebElement done;
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at(i).attribute( "id" ).contains( "s_2_2_20" ) )
        {
            changeJS = c.at(i).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at(i).removeAttribute( "selected");
                
                if ( d.at( i ).attribute( "value" ) == "Public" )
                {
                    e = d.at( i );
                }
            }

            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
        
        else if ( c.at(i).attribute( "id" ).contains( "s_2_2_32" ) )
        {
            changeJS = c.at(i).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at(i).removeAttribute("selected");
                
                if ( d.at( i ).attribute( "value" ) == "Schedule For Close" )
                {
                    e = d.at( i );
                }
            }
        
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }

    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScSecond() ) );
    
    mSetSC = false;
}

void UnityPage::setScSecond()
{
    mSetSC = true; 
    
    disconnect( mViewFrame, 0, 0, 0 );

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

    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScThird() ) );
    
    mSetSC = false; 
}

void UnityPage::setScThird()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    mSetSC = true;
    QString changeJS;
    
    disconnect( mViewFrame, 0, 0, 0 );

    QWebElementCollection rc = mViewFrame->findAllElements( "input" );
    
    for ( int i = 0; i < rc.count(); ++i ) 
    {  
        if ( ( rc.at(i).attribute( "id" ).contains( "s_2_2" ) ) && ( rc.at(i).attribute( "tabindex" ).contains( "2008" ) ) )
        {
            changeJS = rc.at( i ).attribute( "onchange" );
            rc.at( i ).setAttribute( "value", mScDateTime.toString( "M/d/yyyy hh:mm:ss AP" ) );
        }
    }
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setScFourth() ) );
    
    mViewFrame->evaluateJavaScript( changeJS );
    
    mSetSC = false;
}
    
void UnityPage::setScFourth()
{
    mSetSC = true;
    
    QWebElementCollection sc = mViewFrame->findAllElements( "a" );
    QString saveJS;
    
    for ( int i = 0; i < sc.count(); ++i ) 
    {  
        if ( sc.at(i).attribute( "id" ).contains( "s_2_1_14" ) )
        {
            saveJS = sc.at(i).attribute( "href" ).remove( "JavaScript:" );
        }
    }

    //mViewFrame->evaluateJavaScript( saveJS );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( unsetJsConfirm() ) );
    
    connectShortcuts();
    mSetSS = false;
}
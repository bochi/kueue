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

void UnityPage::setStatus( const QString& status )
{
    emit pageErbert();
    mNewStatus = status;
    mNoJsConfirm = true;
    
    if ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Activities" )
    {
        setStatusFirst();
    }
    else
    {
        mSetStatus = true;
        querySR( mCurrentSR );
    }
}

void UnityPage::setStatusFirst()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ),
             this, SLOT( setStatusSecond() ) );   
    
    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QString changeJS;
    QWebElement done;
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at(i).attribute( "id" ).contains( "s_1_1_93" ) )
        {
            changeJS = c.at(i).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute("selected");
                
                if ( d.at( i ).attribute( "value" ) == mNewStatus )
                {
                    e = d.at( i );
                }
            }

            e.setAttribute( "selected", "Yes" );
            mNewStatus.clear();
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }   
}

void UnityPage::setStatusSecond()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( actionDone() ) );
    
    mSetStatus = false;
    saveCurrentSR();
}
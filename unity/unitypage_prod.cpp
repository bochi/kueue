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

void UnityPage::fillOutProduct( const QString& pf, const QString& p, QString pt, const QString& comp )
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ),
             this, SLOT( fillOutProductNext() ) );   
    
    mProduct = p;
    mComponent = comp;

    QWebElementCollection c = mViewFrame->findAllElements( "input,select" );
    
    for ( int i = 0; i < c.count(); ++i ) 
    {  
        if ( ( c.at(i).attribute( "tabindex" ) == "1024" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith("trackChange") ) )
        {
            c.at(i).setAttribute( "value", pf );
        }
    
        if ( ( c.at(i).attribute( "tabindex" ) == "1025" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            c.at(i).setAttribute( "value", "" );
        }
        
        if ( pt.isNull() )
        {
            pt = "Configuration";
        }
        
        if ( ( c.at(i).attribute( "tabindex" ) == "1034" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            QWebElement e;
            QWebElementCollection d = c.at(i).findAll( "*" );
            QString changeJS = c.at(i).attribute( "onchange" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute( "selected");
                
                if ( d.at( i ).attribute( "value" ) == pt )
                {
                    e = d.at( i );
                }
            }
            
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
        
        if ( ( c.at(i).attribute( "tabindex" ) == "1035" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            c.at(i).setAttribute( "value", "" );
        }
    }
    
    saveCurrentSR();
}

void UnityPage::fillOutProductNext()
{
    disconnect( mViewFrame, 0, 0, 0 );
   
    QWebElementCollection c = mViewFrame->findAllElements( "input" );
    QString j;
    
    for ( int i = 0; i < c.count(); ++i ) 
    {  
        if ( ( c.at(i).attribute( "tabindex" ) == "1025" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            c.at(i).setAttribute( "value", mProduct );
        }
        
        if ( ( c.at(i).attribute( "tabindex" ) == "1035" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            c.at(i).setAttribute( "value", mComponent );
        }
    }
   
    saveCurrentSR();
}
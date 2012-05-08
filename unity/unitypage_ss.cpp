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

void UnityPage::setSS( const QString& sr )
{
    if ( sr == "NONE" )
    {
        mSsDialog = new SsDialog( this, mCurrentSR );
    }
    else
    {
        mSsDialog = new SsDialog( this, sr );
    }
    
    connect( mSsDialog, SIGNAL( accepted() ),
             this, SLOT( setSSconfirmed() ) );
    
    connect( mSsDialog, SIGNAL( rejected() ),
             this, SLOT( setSSrejected() ) );
    
    mSsDialog->exec();
}

void UnityPage::setSSconfirmed()
{
    emit pageErbert( "Setting Solution Suggested for SR#" + mSsDialog->sr() );
    
    if ( ( ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Activities" ) ||
           ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Related SRs" ) ) &&
           ( mCurrentSR == mSsDialog->sr() ) )
    {
        mSetSS = true;
        newActivity();
    }
    else
    {
        mSetSS = true;
        querySR( mSsDialog->sr() );
    }
}

void UnityPage::setSSrejected()
{
    delete mSsDialog;
}

void UnityPage::setSSfirst()
{   
    mNoJsConfirm = true;
    
    disconnect( mViewFrame, 0, 0, 0 );

    QString changeJS;
    QString audienceID;
    QString activityID;
    
    if ( mIsCr )
    {
        audienceID = "s_5_2_23";
        activityID = "s_5_2_32";
    }
    else
    {
        audienceID = "s_2_2_20";
        activityID = "s_2_2_32";
    }
    
    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QWebElement done;
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at(i).attribute( "id" ).contains( audienceID ) )
        {
            changeJS = c.at(i).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at(i).removeAttribute("selected");
                
                if ( d.at( i ).attribute( "value" ) == "Internal" )
                {
                    e = d.at( i );
                }
            }

            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
        
        else if ( c.at(i).attribute( "id" ).contains( activityID ) )
        {
            changeJS = c.at(i).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at(i).removeAttribute("selected");
                
                if ( d.at( i ).attribute( "value" ) == "Solution Suggested" )
                {
                    e = d.at( i );
                }
            }
        
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }

    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setSSsecond() ) );
    
    mSetSS = false;
    
    if ( mIsCr )
    {
        setSSsecond();
    }
}

void UnityPage::setSSsecond()
{
    mSetSS = true; 
    
    disconnect( mViewFrame, 0, 0, 0 );

    QString changeJS;
    QString statusID;
    
    if ( mIsCr )
    {
        statusID = "s_5_2_35";
    }
    else
    {
        statusID = "s_2_2_35";
    }
    
    QWebElementCollection c = mViewFrame->findAllElements( "select" );
    QWebElement e;
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at(i).attribute( "id" ).contains( statusID ) )
        {
            changeJS = c.at( i ).attribute( "onchange" );
            
            QWebElementCollection d = c.at(i).findAll( "*" );
            
            for ( int i = 0; i < d.count(); ++i )
            {
                d.at( i ).removeAttribute( "selected" );
                
                if ( d.at( i ).attribute( "value" ) == "Done" )
                {
                    e = d.at( i );
                }
            }
            
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }

    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( setSSthird() ) );
    
    mSetSS = false;
    
    if ( mIsCr )
    {
        setSSthird();
    }
}

void UnityPage::setSSthird()
{
    mSetSS = true;
    QString commentID;
    QString commentTab;
    
    if ( mIsCr )
    {
        commentID = "s_5_2_38";
        commentTab = "3997";
    }
    else
    {
        commentID = "s_2_2";
        commentTab = "2014";
    }
    
    disconnect( mViewFrame, 0, 0, 0 );

    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( actionDone() ) );
    
    QWebElementCollection rc = mViewFrame->findAllElements( "textarea" );
    
    for ( int i = 0; i < rc.count(); ++i ) 
    {  
        if ( ( rc.at(i).attribute( "id" ).contains( commentID ) ) && ( rc.at(i).attribute( "tabindex" ).contains( commentTab ) ) )
        {
            rc.at(i).setInnerXml( mSsDialog->ssText() );
        }
    }
    
    saveCurrentActivity();
    delete mSsDialog;
    mSetSS = false;
}

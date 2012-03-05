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

void UnityPage::addNote()
{
    mNoteDialog = new NoteDialog( this, mCurrentSR );
     
    connect( mNoteDialog, SIGNAL( addNoteAccepted() ), 
             this, SLOT( addNoteAccepted() ) );
    
    connect( mNoteDialog, SIGNAL( rejected() ),
             this, SLOT( addNoteRejected() ) );
    
    mNoteDialog->exec();
}

void UnityPage::addNoteAccepted()
{
    emit pageErbert();
    
    mAddNote = true;
    
    querySR( mNoteDialog->sr() );
}

void UnityPage::addNoteRejected()
{
    delete mNoteDialog;
}

void UnityPage::addNoteFirst()
{
    mNoJsConfirm = true;
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( addNoteSecond() ) );

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
                
                if ( ( d.at( i ).attribute( "value" ) == "Public" ) && ( mNoteDialog->type() == NoteDialog::Public ) )
                {
                    e = d.at( i );
                }
                
                if ( ( d.at( i ).attribute( "value" ) == "Internal" ) && ( mNoteDialog->type() == NoteDialog::Internal ) )
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
                d.at(i).removeAttribute( "selected" );
                
                if ( d.at( i ).attribute( "value" ) == mNoteDialog->noteType() )
                {
                    e = d.at( i );
                }
            }
        
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }
    
    mAddNote = false;
}

void UnityPage::addNoteSecond()
{
    mAddNote = true; 
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished(bool) ),
             this, SLOT( addNoteThird() ) );

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
                
                if ( d.at( i ).attribute( "value" ) == "Done" )
                {
                    e = d.at( i );
                }
            }
            
            e.setAttribute( "selected", "Yes" );
            mViewFrame->evaluateJavaScript( changeJS );
        }
    }
    
    mAddNote = false; 
}

void UnityPage::addNoteThird()
{
    mAddNote = true;
    
    disconnect( mViewFrame, 0, 0, 0 );

    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( actionDone() ) );
        
    QWebElementCollection rc = mViewFrame->findAllElements( "textarea" );
    
    for ( int i = 0; i < rc.count(); ++i ) 
    {  
        if ( ( rc.at(i).attribute( "id" ).contains( "s_2_2" ) ) && ( rc.at(i).attribute( "tabindex" ).contains( "2014" ) ) )
        {
            rc.at(i).setInnerXml( mNoteDialog->comment() );
        }
        
        if ( ( rc.at(i).attribute( "id" ).contains( "s_2_2" ) ) && ( rc.at(i).attribute( "tabindex" ).contains( "2005" ) ) )
        {
            rc.at(i).setInnerXml( mNoteDialog->briefDesc() );
        }
    }
    
    delete mNoteDialog;
    mAddNote = false;
    saveCurrentActivity();
}

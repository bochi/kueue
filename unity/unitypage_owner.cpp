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
/*
void UnityPage::setSubowner( const QStringList& sr, const QString& so )
{
    mSubownerOwner = so;
    mSubownerSR = sr.at( 0 );

    emit pageErbert( "Setting Subowner for SR#" + mSubownerSR + " to " + so.toUpper() );
  
    mNoJsConfirm = true;
    
    if ( ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Activities" ) ||
         ( mViewFrame->findFirstElement( "title" ).toInnerXml() == "Service Request Related SRs" ) &&
         ( mCurrentSR == mSubownerSR ) )
    {
        setSubownerJob();
    }
    else
    {
        mSetSubowner = true;
        querySR( mSubownerSR );
    }
}*/

void UnityPage::setSubownerJob()
{
    QWebElementCollection c = mViewFrame->findAllElements( "input" );
    QString j;
    
    for ( int i = 0; i < c.count(); ++i ) 
    {  
        if ( ( c.at(i).attribute( "tabindex" ) == "1025" ) && 
             ( c.at(i).attribute( "onchange" ).startsWith( "trackChange" ) ) )
        {
            c.at(i).setAttribute( "value", mProduct );
        }
    }
   
    mSetSubowner = false;
    saveCurrentSR();
}

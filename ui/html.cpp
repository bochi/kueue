/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>

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

#include "html.h"
#include "settings.h"
#include "kueue.h"
#include "nsa/nsa.h"

QString HTML::styleSheet()
{
    return QString( "<head><style type='text/css'>"
                    "#Head1 {" 
                    "    position: absolute;"
                    "    z-index: 1; top: 0px;"
                    "    left: 0px; width: 50px;"
                    "    visibility: visible;"
                    "}"
                    
                    "#logo {"
                    "    position: absolute;"
                    "    z-index: 2;"
                    "    top: 0px;"
                    "    right: 5px;"
                    "    width: 175px;"
                    "    height: 80px;"
                    "    visibility: visible;"
                    "}"
                    
                    "#logo1 {"
                    "    position: absolute;"
                    "    z-index: 3;"
                    "    top: 0px;"
                    "    right: 0px;"
                    "    width: 12px;"
                    "    height: 80px;"
                    "    visibility: visible;"
                    "}"
                   
                    "#Apptitle {"
                    "    position: absolute;"
                    "    overflow:hidden;"
                    "    z-index: 1;"
                    "    top: 0px;"
                    "    left: 10px;"
                    "    right: 20px;"
                    "    height: 80px;"
                    "    visibility: visible;"
                    "}"
                    
                    "body {"
                    "    background: white url('qrc:/images/filler.gif') repeat-x 0% 0%;"
                    "}"
                    
                    ".apptitle1 {"
                    "    color: white;"
                    "    text-decoration: none;"
                    "    font-family: arial,helvetica,sans-serif;"
                    "    font-weight: normal;"
                    "    font-size: 1.7em;"
                    "    line-height: 2.7em;"
                    "    background: url('qrc:/images/bg.gif') repeat-x 0% 0%;"
                    "}"
                    
                    ".apptitle2 {"
                    "    color: white;"
                    "    font-family: arial,helvetica,sans-serif;"
                    "    text-decoration: none;"
                    "    font-weight: normal;"
                    "    font-size: 0.9em;"
                    "    line-height: 1.2em;"
                    "    right: 40px;"
                    "    background: url('qrc:/images/logo_im.gif') background-repeat:no-repeat;"
                    "}"
                    
                    "#abstand {"
                    "    margin-bottom:10px;"
                    "}"
                    
                    "#content {"
                    "    margin-top:90px;"
                    "    font-family: arial,helvetica,sans-serif;"
                    "}"
                    
                    "hr {"
                    "    background: url('qrc:/images/bg.gif') repeat-x 0% 0%;"
                    "    height: 1px;"
                    "    margin-top: 2px;"
                    "    margin-bottom: 10px;"
                    "}"
                    
                    "table {}"
                    
                    "td {}"
                    
                    ".qmonheader {"
                    "    background: url('qrc:/images/bg.gif') repeat-x 0% 0%;"
                    "    padding:0;"
                    "    margin:0;"
                    "    text-align: center;"
                    "    color: white;"
                    "    font-weight: bold;"
                    "    font-size: 1.2em;"
                    "    line-height: 1.2em;"
                    "    width: 100%;"
                    "    height: 55px;"
                    "}"
                    
                    "p {" 
                    "    margin-bottom:10px;"
                    "}"
                    
                    ".qph {"
                    "    font-family: arial,helvetica,sans-serif;"
                    "    font-weight: bold;"
                    "    font-size: 1.2em;"
                    "    margin-bottom: 1px;"
                    "}"
                    
                    "a {"
                    "    color:#333;"
                    "    text-decoration:none;"
                    "    -khtml-user-select: none;"
                    "}"
                    
                    ".dotlinehoriz {"
                    "    background-image: url('qrc:/images/dotlinebg_horiz.gif');"
                    "    height:1px;"
                    "    line-height:1px;"
                    "    overflow:hidden;"
                    "}"
                    
                    ".dotlinevert {"
                    "    background-image: url('qrc:/images/dotlinebg_vert.gif');"
                    "    width:1px;"
                    "}"
                    
                    ".gadgetHead {"
                    "    font:14px/18px arial,helvetica,sans-serif;"
                    "}"
                    
                    ".gadgetText {"
                    "    font:14px/24px arial,helvetica,sans-serif;"
                    "}" 
                  
                    "</style></head><body>" );
}

QString HTML::pageHeader( const QString& engineer, int total )
{
    QString ph;
    
    ph += QString( "<div id='Head1'>"
                   "    <img src='qrc:/images/gfx_top_in.gif' width='32' height='80' border='0'/>"
                   "</div>"
                   
                   "<div id='Apptitle' style='left:30px; right:60px; top:0px; height: 80px'>"
                   "    <div id='AppTitleDiv' class='apptitle1' style='padding-top: 3px; padding-bottom: 3px; width: 100%'>"
                   "        <b>" + engineer + "'s Queue</b>"
                   "    </div>"
                   "</div>"
                   
                   "<div id='logo' style='z-index:2'>"
                   "    <img src='qrc:/images/logo_im.gif' width='175' height='80' border='0'>"
                   "</div>"
                   
                   "<div id='Apptitle' style='left:20px; right:20px; top:0px; height: 80px; z-index: 4'>"
                   "    <div id='AppTitleDiv' class='apptitle2' style='text-align:right; padding-top: 21px; "
                   " padding-bottom: 3px; width: 100%; z-index: 3'>"
                   "         SRs total:&nbsp;" + QString::number( total ) + "<br>" );

    if ( Settings::sortAge() )
    {
        ph += "Sorted by age"; 
    }
    else 
    {    
        ph += "Sorted by last activity";
    }

    ph += QString( "<br>"
                   "    </div>"
                   "</div>"
                   "<div id='logo' style='z-index:2'>"
                   "    <img src='qrc:/images/logo_im.gif' width='175' height='80' border='0'>"
                   "</div>"
                   "<div id='content'>" );
    
    return ph;
}

QString HTML::SRTable( SR* sr )
{
    QString srtab;
    QString t;
    QStringList list = sr->todoList();
      
    srtab += QString (  "<table id='" + sr->id() + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
                            "<tr>"
                                "<td colspan='3'>"
                                    "<div class='dotlinehoriz'>"
                                        "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''>"
                                    "</div>"
                                "</td>"
                            "</tr>"
                            "<tr>"
                                "<td width='1' rowspan='4' class='dotlinevert'>"
                                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''>"
                                "</td>"
                                "<td width='100%' bgcolor='#E8E8E8'>"
                                    "<a href='sr://" + sr->id() + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'>"
                                            "<td class='gadgetHead'>" );

    if ( sr->isCR() ) 
    {    
        srtab += QString("<b> CR# " + sr->id() + " - " + sr->briefDesc() + "</b>");
    }
    else 
    {    
        srtab += QString ( "<b> SR# " + sr->id() + " - " + sr->briefDesc() + "</b>");
    }
    
    if ( sr->display() == "none" )
    {
        srtab += QString( "<div id='" + sr->id() + "_hinfo' style='display:block'><i>" + sr->status() + 
                          " - Age: " + QString::number( sr->age() ) + " days - Last update: " + 
                          QString::number( sr->lastUpdateDays() ) + " days ago</div>" );
    }
    else if ( sr->display() == "block" )
    {
        srtab += QString( "<div id='" + sr->id() + "_hinfo' style='display:none'><i>" + sr->status() + 
                          " - Age: " + QString::number( sr->age() ) + " days - Last update: " + 
                          QString::number( sr->lastUpdateDays() ) + " days ago</div>" );
    }
    else
    {
        srtab += QString( "<div id='" + sr->id() + "_hinfo' style='display:block'><i>" + sr->status() + 
                          " - Age: " + QString::number( sr->age() ) + " days - Last update: " + 
                          QString::number( sr->lastUpdateDays() ) + " days ago</div>" );
    }
 
    srtab += QString (  "</td></a>"
                        "<td align='right'>"
                            "<a href='arrow://" + sr->id() + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a>"
                        "</td>"
                    "</tr>"
                "</table>"
            "</td>"
            "<td width='1' rowspan='4' class='dotlinevert'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
            "</td>"
        "</tr>"
        "<tr>"
            "<td colspan='3'>"
                "<div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></img>"
                "</div>"
            "</td>"
        "</tr>"
    "</table>"
    "<a href='sr://" + sr->id() + "'><table id='" + sr->id() + "_body' width='100%' cellspacing='0' cellpadding='0' border='0' style='display:" );
  
    if( sr->display().isEmpty() ) 
    {   
        srtab+="none";
    }
    else if ( !sr->display().isEmpty() )
    {    
        srtab+=QString( sr->display() );
    }
    else
    {
        srtab+="block";
    }
  
    srtab+=(    "'><tr>"
                "<td width='1' rowspan='4' class='dotlinevert'>"
                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
                "</td>"
                "<td width='100%'>"
                    "<table cellpadding='0' cellspacing='0' border='0'>"
                        "<tr>"
                            "<td class='gadgetText'>"
                                "<tr>"
                                    "<td class='gadgetText'>Customer&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                    "<td class='gadgetText'>" + sr->customer() + " (" +sr->contact()+")</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Age</td>"
                                    "<td class='gadgetText'>" + QString::number( sr->age() ) + " days</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Status</td>"
                                    "<td class='gadgetText'>" + sr->status() + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Last Update</td>"
                                    "<td class='gadgetText'>" + QString::number( sr->lastUpdateDays() ) + " days ago</td>"
                                "</tr>" );

    if ( Settings::showSS() )
    {
        if ( sr->ss() ) 
        {
            srtab+=( "<tr>"
                     "<td class='gadgetText'>Solution Suggested&nbsp;&nbsp;</td>"
                 "<td class='gadgetText'>Yes</td>"
             "</tr>" );
        }
        else
        {
            srtab+=( "<tr>"
                        "<td class='gadgetText'>Solution Suggested&nbsp;&nbsp;</td>"
                        "<td class='gadgetText'>No</td>"
                     "</tr>" );
        }
    }

    if( !( !( Settings::todoShowEsc() ) && 
        !(Settings::todoShowStat() ) && 
        !( Settings::todoShowUp() ) ) )
    {                
        srtab+= ( "<tr><td class='gadgetText' valign='top'>TODO</td><td class='gadgetText'>" );
  
        for (int i = 0; i < list.size(); ++i) 
        {
            t = QString( list.at(i).toUtf8() );
  
            if( Settings::todoShowSmilies() ) 
            {
                if( t.startsWith( "[1]" ) )
                {    
                    srtab+=( "<img src='qrc:/images/sad.gif'></img> " + t.remove( "[1]" ) + "<br>");
                }
                else if( t.startsWith( "[2]" ) )
                {
                    srtab+=( "<img src='qrc:/images/und.gif'></img> " + t.remove( "[2]" ) + "<br>");
                }
                else if( t.startsWith( "[3]" ) )
                {
                    srtab+=( "<img src='qrc:/images/yay.gif'></img> " + t.remove( "[3]" ) + "<br>");
                }
            }
            else
            {
                if( t.startsWith( "[1]" ) )
                {
                    srtab+=( "&#8226; " + t.remove( "[1]" ) + "<br>");
                }
                else if( t.startsWith( "[2]" ) )
                {
                    srtab+=( "&#8226; " + t.remove( "[2]" ) + "<br>");
                }
                else if( t.startsWith( "[3]" ) )
                {
                    srtab+=( "&#8226; " + t.remove( "[3]" ) + "<br>");
                }
            }
        }
  
    srtab +=( "</td></tr>" );
  
    }
    
    srtab+= (   "</table></a></a></td><td width='1' rowspan='4' class='dotlinevert'><img src='qrc:/images/spacer.gif'"
                "width='1' height='1' border='0' alt=''></td></tr><tr><td colspan='3'><div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></div></td></tr></table></a><div id='abstand'></div>" );  

    return srtab;
}

QString HTML::pageFooter() 
{
    QDateTime now = QDateTime::currentDateTime();
    
    return QString ( "<div id='footer'>Generated by kueue at " + now.toString( "yyyy-MM-dd hh:mm:ss" ) + "</div>" );
} 

QString HTML::qmonPageHeader()
{
    QString ph;
    
    ph += QString( "<div id='Head1'><img src='qrc:/images/gfx_top_in.gif' width='32' height='80' border='0'/></div>"
                   "<div id='Apptitle' style='left:30px; right:12px; top:0px; height: 80px'>"
                   "<a id='AppTitleA' style='text-decoration: none'>"
                   "<div id='AppTitleDiv' class='apptitle1' style='padding-top: 3px; padding-bottom: 3px; width: 100%'>"
                   "<b>Queue Monitor</b>"
                   "</div></div></a>"
                   "<div id='logo1' style='z-index:3'>"
                   "<img src='qrc:/images/logo_im_end.gif' width='12' height='80' border='0'></div>" 
                   "<div id='content'>" );

    return ph;
}

QString HTML::qmonTableHeader( const QString& queue )
{
    QString ph;
 
    ph += QString ( "<p class='qph'><b>" + queue + "</b></p><hr>" );

    return ph;
}

QString HTML::qmonSrInQueue( SiebelItem* si )
{
    QString srtab;
  
    srtab += QString (  "<table id='" + si->id + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
                            "<tr>"
                                "<td colspan='3'>"
                                    "<div class='dotlinehoriz'>"
                                        "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''>"
                                    "</div>"
                                "</td>"
                            "</tr>"
                            "<tr>"
                                "<td width='1' rowspan='4' class='dotlinevert'>"
                                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''>"
                                "</td>"
                                "<td width='100%' bgcolor='#E8E8E8'>"
                                    "<a href='sr://" + si->id + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'>"
                                            "<td class='gadgetHead'>" );

 
    if (  si->isCr )
    {
        srtab += QString ( "<b>CR# " );
    }
    else
    {
        srtab += QString ( "<b>SR# " );
    }
    
    srtab += QString( si->id );
    
    if ( si->isChat )
    {
        srtab += QString( "<img src='qrc:/images/chat.png'></img>" );
    }    
    
    if ( ( si->highValue ) || ( si->critSit ) )
    {
        srtab += QString( "<img src='qrc:/images/obacht.png'></img>" );
    }   
    
    srtab += QString( "&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;" + si->geo + " (" + si->hours + ")&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;" );
    
    if ( si->severity == "High" )
    {
        srtab += QString( "<font color='DarkRed'>High</font>" );
    }
    else if ( si->severity == "Urgent" )
    {
        srtab += QString( "<font color='DarkBlue'>Urgent</font>" );
    }
    else
    {
        srtab += QString( si->severity );
    }
       
    srtab += QString (  "&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;Age: " + timeString( si->age ) );
    srtab += QString (  "&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;Time in queue: " + timeString( si->timeInQ ) );
   
    if ( si->slaLeft > 0 )      
    {
        srtab += QString ( "&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;&nbsp;SLA left: " + timeString( si->slaLeft ) );
    }
    
    srtab += QString (  "</b><br><i>" +  si->bdesc + "</i>"
                        "</td></a>"
                        "<td align='right'>"
                            "<a href='arrow://" + si->id + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a><br>"
                        "</td>"
                    "</tr>"
                "</table>"
            "</td>"
            "<td width='1' rowspan='4' class='dotlinevert'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
            "</td>"
        "</tr>"
        "<tr>"
            "<td colspan='3'>"
                "<div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></img>"
                "</div>"
            "</td>"
        "</tr>"
    "</table>"
    "<a href='sr://" + si->id + "'><table id='" + si->id + "_body' width='100%' cellspacing='0' cellpadding='0' border='0' style='display:" );
  
    if ( si->display.isEmpty() )
    {
        srtab += "none";
    }
    else
    {
        srtab += si->display;
    }
  
    srtab+=(    "'><tr>"
                "<td width='1' rowspan='4' class='dotlinevert'>"
                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
                "</td>"
                "<td width='100%'>"
                    "<table cellpadding='0' cellspacing='0' border='0'>"
                        "<tr>"
                            "<td class='gadgetText'>" );

    if ( si->isCr )
    {
        srtab += QString( "<tr>"
                            "<td class='gadgetText'>Opened by&nbsp;&nbsp;&nbsp;</td>"
                            "<td class='gadgetText'>" + si->creator + "</td>"
                          "</tr>" );
    }
    else
    {
        srtab += QString( "<tr>"
                            "<td class='gadgetText'>Customer</td>"
                            "<td class='gadgetText'>" + si->customer + "</td>"
                         "</tr>" );
    }
    
    if ( si->isChat )
    {
        srtab += QString(       "<tr>"
                                   "<td class='gadgetText'>Bomgar Queue/Owner&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>" + si->bomgarQ + "</td>"
                                "</tr>" );
    }
    
    srtab += QString(           "<tr>"
                                   "<td class='gadgetText'>Status</td>"
                                   "<td class='gadgetText'>" + si->status + "</td>"
                                "</tr>" );
    if ( !si->isCr )
    {
        srtab += QString(       "<tr>"
                                   "<td class='gadgetText'>Contract</td>"
                                   "<td class='gadgetText'>" + si->contract + "</td>"
                                "</tr>" );
    }
                                
     srtab += QString(          "<tr>"
                                   "<td class='gadgetText'>Last Update&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>" + QString::number( secDays( si->lastAct ) ) + " days ago</td>"
                                "</tr>" );
    
    if ( si->highValue )
    {
        srtab += QString(       "<tr>"
                                   "<td class='gadgetText'>High Value&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>Yes</td>"
                                "</tr>" );
    }
    else
    {
        srtab += QString(       "<tr>"
                                    "<td class='gadgetText'>High Value&nbsp;&nbsp;&nbsp;</td>"
                                    "<td class='gadgetText'>No</td>"
                                "</tr>" );
    }

    if ( si->critSit )
    {
        srtab += QString(       "<tr>"
                                    "<td class='gadgetText'>CritSit&nbsp;&nbsp;&nbsp;</td>"
                                    "<td class='gadgetText'>Yes</td>"
                                "</tr>" );
    }
    else
    {
        srtab += QString(       "<tr>"
                                    "<td class='gadgetText'>CritSit&nbsp;&nbsp;&nbsp;</td>"
                                    "<td class='gadgetText'>No</td>"
                                "</tr>" );
    }
    
    srtab +=( "</td></tr>" );
  
    srtab+= (   "</table></a></a></td><td width='1' rowspan='4' class='dotlinevert'><img src='qrc:/images/spacer.gif'"
                "width='1' height='1' border='0' alt=''></td></tr><tr><td colspan='3'><div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></div></td></tr></table><div id='abstand'></div></a>" );  
  
    return srtab;
}

QString HTML::qmonTableFooter()
{
    return ( "</div></table>" );  
}

QString HTML::statsPageHeader()
{
    QString ph;
    
    ph += QString( "<div id='Head1'><img src='qrc:/images/gfx_top_in.gif' width='32' height='80' border='0'/></div>"
                   "<div id='Apptitle' style='left:30px; right:12px; top:0px; height: 80px'>"
                   "<a id='AppTitleA' style='text-decoration: none'>"
                   "<div id='AppTitleDiv' class='apptitle1' style='padding-top: 3px; padding-bottom: 3px; width: 100%'>"
                   "<b>Quarterly Statistics</b>"
                   "</div></div></a>"
                   "<div id='logo1' style='z-index:3'>"
                   "<img src='qrc:/images/logo_im_end.gif' width='12' height='80' border='0'></div></div>"
                   "<div id='content'>" );

    return ph;
}

QString HTML::csatTableHeader( int rts, int eng, int sr )
{
    QString ph;
    
    if ( ( rts == 0 ) && 
         ( eng == 0 ) && 
         ( sr == 0 ) )
    {
        ph += QString ( "<p class='qph'><b>Customer Satisfaction</b></p><hr>" 
                        "No data available at present<br><br>" );
    }
    else
    {
        ph += QString ( "<p class='qph'><b>Customer Satisfaction</b></p><hr>"
                        "Resolved to Satisfaction: " + QString::number( rts ) + "%<br>"
                        "Average SR Satisfaction: " + QString::number( sr ) + "<br>"
                        "Average Engineer Satisfaction: " + QString::number( eng ) + "<br><br>" );
    }
    
    return ph;
}


QString HTML::csatTable( CsatItem* i )
{
    QString srtab;
    
    srtab += QString (  "<table id='" + i->sr + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
                            "<tr>"
                                "<td colspan='3'>"
                                    "<div class='dotlinehoriz'>"
                                        "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''>"
                                    "</div>"
                                "</td>"
                            "</tr>"
                            "<tr>"
                                "<td width='1' rowspan='4' class='dotlinevert'>"
                                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''>"
                                "</td>"
                                "<td width='100%' bgcolor='#EDEEEC'>"
                                    "<a href='sr://" + i->sr + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'><td class='gadgetHead'>" );
    
    if ( i->rts ) 
    {
        srtab += QString( "<img src='qrc:/images/yay.gif'></img> " );
    }
    else
    {
        srtab += QString( "<img src='qrc:/images/sad.gif'></img> " );
    }
    
    srtab += QString( "<b>SR#" + i->sr + "</b> - " + i->bdesc );
                                            
    srtab += QString( "</td></a>"
                      "<td align='right'>"
                                    "<a href='arrow://" + i->sr + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a><br>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</td>"
                    "<td width='1' rowspan='4' class='dotlinevert'>"
                        "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td colspan='3'>"
                        "<div class='dotlinehoriz'>"
                            "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></img>"
                        "</div>"
                    "</td>"
                "</tr>"
            "</table>"
            "<a href='sr://" + i->sr + "'><table id='" + i->sr + "_body' width='100%' cellspacing='0' cellpadding='0' "
            "border='0' style='display:none'><tr>"
                  "<td width='1' rowspan='4' class='dotlinevert'>"
                       "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
                  "</td>"
                  "<td width='100%'>"
                        "<table cellpadding='0' cellspacing='0' border='0'>"
                            "<tr>"
                                "<td class='gadgetText'>"
                                "<tr>"
                                    "<td class='gadgetText'>Customer</td>"
                                    "<td class='gadgetText'>" + i->customer + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Resolved to Satisfaction&nbsp;&nbsp;&nbsp;&nbsp;</td>" );
                                    
    if ( i->rts )
    {
        srtab += QString( "<td class='gadgetText'>Yes</td>" );
    }
    else
    {
        srtab += QString( "<td class='gadgetText'>No</td>" );
    }
    
    srtab += QString (          "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>SR Satisfaction</td>"
                                    "<td class='gadgetText'>" + i->srsat + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Engineer Satisfaction</td>"
                                    "<td class='gadgetText'>" + i->engsat + "</td>"
                                "</tr>" );

    srtab +=( "</td></tr>" );
  
    srtab+= (   "</table></a></a></td><td width='1' rowspan='4' class='dotlinevert'><img src='qrc:/images/spacer.gif'"
                "width='1' height='1' border='0' alt=''></td></tr><tr><td colspan='3'><div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></div></td></tr></table>" );  
    
    srtab += QString( "<div id='abstand'></div>" );
  
    return srtab;
}

QString HTML::closedTableHeader( int c, int a )
{
    QString ph;
 
    ph += QString ( "<p class='qph'><b>Closed SRs/Time to solution</b></p><hr>"
                    "You closed " + QString::number( c ) + " SRs so far<br>"
                    "Average time to solution: " + QString::number( a ) + " days<br><br>" );

    return ph;
}

QString HTML::closedTable( TtsItem* ci )
{
    QString srtab;
    
    srtab += QString (  "<table id='" + ci->sr + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
                            "<tr>"
                                "<td colspan='3'>"
                                    "<div class='dotlinehoriz'>"
                                        "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''>"
                                    "</div>"
                                "</td>"
                            "</tr>"
                            "<tr>"
                                "<td width='1' rowspan='4' class='dotlinevert'>"
                                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''>"
                                "</td>"
                                "<td width='100%' bgcolor='#EDEEEC'>"
                                    "<a href='sr://" + ci->sr + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'><td class='gadgetHead'><b>SR#" + ci->sr + "</b> - " + 
                                        QString::number( ci->tts ) + " days - " + ci->bdesc );
    

      
    srtab+= (   "</table></a></a></td><td width='1' rowspan='4' class='dotlinevert'><img src='qrc:/images/spacer.gif'"
                "width='1' height='1' border='0' alt=''></td></tr><tr><td colspan='3'><div class='dotlinehoriz'>"
                "<img src='qrc:/images/spacer.gif' width='1' height='1' alt=''></div></td></tr></table>" );  
    
    srtab += QString( "<div id='abstand'></div>" );
                    
    return srtab;
}

QString HTML::nsaPageHeader( NSASummaryItem summary )
{
    QString out;
    
    out +=( "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>"
            "<html xmlns='http://www.w3.org/1999/xhtml'><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
            "<title>NSA Report</title><style type='text/css'>"

            "body {"
                    "background: #666666;"
                    "margin: 0;"
                    "padding: 0;"
                    "text-align: center;"
                    "color: #000000;"
                    "font-family: Verdana, Arial, Helvetica, sans-serif;"
                    "font-size: 12px;"
            "}"

            ".oneColFixCtrHdr #container {"
                    "width: 1000px;"
                    "background: #FFFFFF;"
                    "border: 1px solid #000000;"
                    "text-align: left;"
                    "padding: 0px;"
                    "margin-top: 0;"
                    "margin-right: auto;"
                    "margin-bottom: 0;"
                    "margin-left: auto;"
            "}"
            
            ".oneColFixCtrHdr #header {"
                    "font-size: 18px;"
            "}"
            
            ".oneColFixCtrHdr #header h1 {"
                    "margin: 0;"
                    "padding: 10px 0;"
            "}"
            
            ".oneColFixCtrHdr #mainContent {"
                    "padding: 0 20px; "
                    "background: #FFFFFF;"
            "}"
            
            ".oneColFixCtrHdr #footer {"
                    "padding: 0 10px; "
                    "background:#DDDDDD;"
            "}"
            
            ".oneColFixCtrHdr #footer p {"
                    "margin: 0; "
                    "padding: 10px 0;"
            "}"
            
            ".oneColFixCtrHdr #container #mainContent h6 {"
                    "color: #06F;"
                    "font-size: 12px;"
            "}"
            
            "</style></head>"
            
            "<body class='oneColFixCtrHdr'><br /><div id='container'><div id='header'>"
            "<table width='100%'><tr><td width='68%'>&nbsp;</td><td width='32%' bgcolor='#0099FF'>&nbsp;</td></tr>"
            "<tr><td>&nbsp;</td><td align='right'><a href='http://support.novell.com/advisor' target='_blank'>Novell Support Advisor</a></td></tr>"
            "</table></div><div id='mainContent'><br><br><table width='100%'>" );
    
    if ( summary.slesversion.isEmpty() )
    {
        out += ( "<tr><td width='16%'><b>Archive file:</b></td><td width='64%'>" + summary.archive + "</td></tr>"
                 "<tr><td>&nbsp;</td></tr></table>"
                 "<b><i>No further info available (most likely supportconfig version is too old)</i></b>" );
    }
    else
    {
        out += ( "<tr><td width='16%'><b>Supportconfig version:</b></td><td width='84%'>" + summary.scriptversion + " (" + summary.scriptdate.toString( "yyyy-MM-dd" ) + ")</td></tr>"
                 "<tr><td><b>Archive file:</b></td><td>" + summary.archive + "</td></tr>"
                 "<tr><td>&nbsp;</td></tr>"
                 "<tr><td><b>Report Run Date:</b></td><td>" + summary.rundate.toString( "yyyy-MM-dd - hh:mm" ) + "</td></tr>"
                 "<tr><td><b>Hostname:</b></td><td>" + summary.hostname + "</td></tr>"
                 "<tr><td>&nbsp;</td></tr>"
                 "<tr><td><b>SLES Version:</b></td><td> SUSE Linux Enterprise Server " + summary.slesversion + " SP " + summary.slessp + "</td></tr>" );
        
        if ( !summary.oesversion.isEmpty() )
        {
            out += ( "<tr><td><b>OES Version:</b></td><td> Open Enterprise Server " + summary.oesversion + " SP " + summary.oessp + "</td></tr>" );
        }
        
        out += ( "<tr><td><b>Architecture:</b></td><td>" + summary.arch + "</td></tr>"
                 "<tr><td>&nbsp;</td></tr>"
                 "<tr><td><b>Kernel:</b></td><td>" + summary.kernel + "</td></tr></table>" );
    }
            
    out += ( "<h6>Results Overview<hr /></h6>" );
    
    return out;
}

QString HTML::nsaTableStart( const QString& table )
{
    return "<strong>" + table + "</strong><table cellpadding='5'>";
}

QString HTML::nsaTableItem( NSATableItem ti )
{
    QString output;
    QList<QStringList> linkList = ti.linkList;
    
    if ( ti.type == NSATableItem::Critical )
    {
        output += "<tr bgcolor='#FF0000'>";
    }
    else if ( ti.type == NSATableItem::Warning )
    {
        output += "<tr bgcolor='#FFFF99'>";
    }
    else if ( ti.type == NSATableItem::Recommended )
    {
        output += "<tr bgcolor='#33CCFF'>";
    }
    
    output += "<td>" + ti.category + "</td>";
    output += "<td>" + ti.desc + "</td>";
    
    for ( int i = 0; i < linkList.size(); ++i )
    {
        output += "<td><a href='" + linkList.at(i).at(1) +"'>" + linkList.at(i).at(0) + "</a></td>";
    }
    
    output += "</tr>";
    
    return output;
}

QString HTML::nsaTableEnd()
{
    return "</table><br>";
}

QString HTML::nsaPageEnd()
{
    return "</html>";
}

QString HTML::timeString( int sec )
{
    int hours;
    int minutes;
    int days( sec / 86400 );
 
    QString d = "Days";
    QString h = "Hours";
    QString m = "Minutes";
    
    if ( days > 0 )
    {
        sec = ( sec - ( days * 86400 ) );
        hours = ( sec / 3600 );
        sec = ( sec - ( hours * 3600 ) );
        minutes = ( sec / 60 );
        
        if ( days == 1 ) d = "Day";
        if ( hours == 1 ) h = "Hour";
        if ( minutes == 1 ) m = "Minute";
        
        return QString( QString::number( days ) + " " + d + " " + QString::number( hours ) + " " + h + 
                        " " + QString::number( minutes ) + " " + m );
    }
    else 
    {
        hours = ( sec / 3600 );
        
        if ( hours > 0 )
        {
            sec = ( sec - ( hours * 3600 ) );
            minutes = ( sec / 60 );        
            
            if ( hours == 1 ) h = "Hour";
            if ( minutes == 1 ) m = "Minute";
            
            return QString( QString::number( hours ) + " " + h + " " + QString::number( minutes ) + " " + m );
        }
        else
        {
            minutes = ( sec / 60 );   
            
            if ( minutes == 1 ) m = "Minute";
            
            return QString( QString::number( minutes ) + " " + m );
        }
    }
}

int HTML::secDays( int sec )
{
    uint now = QDateTime::currentDateTime().toTime_t();
    uint notNow = now - sec;
    QDateTime tmp = QDateTime::fromTime_t( notNow );
    return tmp.daysTo( QDateTime::currentDateTime() );
}

QString HTML::csatDate()
{
    QNetworkReply *reply = Kueue::download( QUrl( Settings::dBServer() + "/custsatdate.asp" ) );
    QEventLoop loop;
 
    QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
    loop.exec();                         
    
    QString data = reply->readAll();
    data.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
    QDateTime t = QDateTime::fromString( data.trimmed(), "M/d/yyyy h:mm:ss" );
    return t.toString( "yyyy-MM-dd" );
}
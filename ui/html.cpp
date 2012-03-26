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

#include "html.h"
#include "settings.h"
#include "kueue.h"
#include "data/dataclasses.h"

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
                    "    padding:5;"
                    "    margin:5;"
                    "    text-align: center;"
                    "    color: white;"
                    "    font-weight: bold;"
                    "    font-size: 1.2em;"
                    "    line-height: 1.2em;"
                    "    width: 100%;"
                    "    height: 55px;"
                    "}"
                    
                    "p {" 
                    "    margin-bottom:5px;"
                    "    margin-top:5px;"
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
                    "    margin-top:10px;"
                    "    margin-bottom:10px;"
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

QString HTML::SRTable( QueueSR sr )
{
    QString srtab;
    QString t;
    
    if ( sr.id == "00000000000" )
    {
        return "Updating...";
    }
    
    srtab += QString (  "<table id='" + sr.id + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
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
                                    "<a href='sr://" + sr.id + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'>"
                                            "<td class='gadgetHead' valign='center' width='15%'>" );

    if ( sr.srtype == "cr" ) 
    {    
        srtab += QString("<p style='line-height:0.8em;'><b>&nbsp;CR# " + sr.id + "&nbsp;</b></p><p style='line-height:0.7em;'><font size='-1'><i>&nbsp;" + sr.status + "</i></p>" );
    }
    else 
    {    
        srtab += QString("<p style='line-height:0.8em;'><b>&nbsp;SR# " + sr.id + "&nbsp;</b>" );
        
        if ( ( sr.highvalue ) || ( sr.critsit ) )
        {
            srtab += QString( "<img src='qrc:/images/obacht.png'></img>" );
        }   
        
        srtab += QString( "</p><p style='line-height:0.7em;'><font size='-1'><i>&nbsp;" + sr.status + "</i></p>" );
    }
    
    srtab += QString( "</font></td><td class='gadgetText' width='60%'><p style='line-height:0.7em;'>" + sr.bdesc + "</p>"
                      + "<p style='line-height:0.5em;'><b><i><font size='-2'>" + sr.cus_account + "</font></p></i></b></td>" );
    
    srtab += QString( "<td width='25%' align='right'><p style='line-height:0.6em;'><font size='-1'>"
                      "Age: " + QString::number( sr.age ) + " days&nbsp;</p><p style='line-height:0.6em;'>"
                      "Last update: " + QString::number( sr.lastUpdateDays ) + " days ago&nbsp;</p></td>" );
    
   
    srtab += QString (  "</td></a>"
                        "<td align='right'>"
                            "<a href='arrow://" + sr.id + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a>"
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
    "<a href='sr://" + sr.id + "'><table id='" + sr.id + "_body' width='100%' cellspacing='0' cellpadding='0' border='0' style='display:" );
  
    if( sr.display.isEmpty() ) 
    {   
        srtab+="none";
    }
    else if ( !sr.display.isEmpty() )
    {    
        srtab+=QString( sr.display );
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
                                    "<td class='gadgetText'>" + sr.cus_account + " (" + sr.cus_firstname + " " + sr.cus_lastname +")</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Age</td>"
                                    "<td class='gadgetText'>" + QString::number( sr.age ) + " days</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Status</td>"
                                    "<td class='gadgetText'>" + sr.status + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Last Update</td>"
                                    "<td class='gadgetText'>" + QString::number( sr.lastUpdateDays ) + " days ago</td>"
                                "</tr>" );

    QStringList list = sr.todoList;
    
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
    }
  
    srtab +=( "</td></tr>" );
    
    srtab += (   "</table></a></a></td><td width='1' rowspan='4' class='dotlinevert'><img src='qrc:/images/spacer.gif'"
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

QString HTML::qmonSrInQueue( QmonSR sr )
{
    QString srtab;
  
    if ( sr.id == "00000000000" )
    {
        return "Updating...";
    }
        
    srtab += QString (  "<table id='" + sr.id + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
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
                                    "<a href='sr://" + sr.id + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'  height='50px'>"
                                            "<td class='gadgetHead' valign='center' width='15%'>" );

 
    if (  sr.isCr )
    {
        srtab += QString ( "<p style='line-height:0.9em;'><b>&nbsp;CR# " );
    }
    else
    {
        srtab += QString ( "<p style='line-height:0.9em;'><b>&nbsp;SR# " );
    }
    
    srtab += QString( sr.id + "</b>" );
    
    if ( sr.isChat )
    {
        srtab += QString( "&nbsp;<img src='qrc:/images/chat.png'></img>" );
    }    
    
    if ( ( sr.highvalue ) || ( sr.critsit ) )
    {
        srtab += QString( "&nbsp;<img src='qrc:/images/obacht.png'></img>" );
    }   
    
    srtab += QString( "</p><b><font size='-1'><p style='line-height:0.6em;'>&nbsp;" + sr.geo + " (" + sr.hours + ") - " );
    
    if ( sr.severity == "High" )
    {
        srtab += QString( "<font color='DarkRed'>High</font>" );
    }
    else if ( sr.severity == "Urgent" )
    {
        srtab += QString( "<font color='DarkBlue'>Urgent</font>" );
    }
    else
    {
        srtab += QString( sr.severity );
    }

    srtab += QString( "</font></b></p></td><td width='60%' valign='center' align='left' class='gadgetText'>" );
   
    if ( sr.cus_account.isEmpty() )
    {
        sr.cus_account = sr.creator;
    }
    
    srtab += QString (  "<p style='line-height:0.7em;'>" +  sr.bdesc.trimmed() + "</p>"
                            "<p style='line-height:0.5em;'><font size='-2'><b><i>" + sr.cus_account.trimmed() + "</b></i></font></p>"
                        "</td></a>"
                        "<td width='25%' align='right' valign='center' class='gadgetText'><font size='-1'>" 
                        "<p style='line-height:0.7em;'>Age: " + timeString( sr.agesec ) + "</p>" );
    
   
        //srtab += QString( "<p style='line-height:0.7em;'>&nbsp;</p>" );
   
    
    srtab += QString( "<p style='line-height:0.7em;'>In Q: " + timeString( sr.timeinqsec ) +"</p>" );
                        
    if ( sr.slasec > 0 )
    {
        srtab += QString( "<p style='line-height:0.7em;'>SLA: " + timeString( sr.slasec ) + "</p></b>" );
    }
                        
                      
        srtab += ( "</font></td><td align='right'>"
                            "<a href='arrow://" + sr.id + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a><br>"
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
    "</table>" );
      
   srtab += (
    "<a href='sr://" + sr.id + "'><table id='" + sr.id + "_body' width='100%' cellspacing='0' cellpadding='0' border='0' style='display:" );
  
    if ( sr.display.isEmpty() )
    {
        srtab += "none";
    }
    else
    {
        srtab += sr.display;
    }
  
    srtab+=(    "'>"
                "<tr>"
                "<td width='1' rowspan='4' class='dotlinevert'>"
                    "<img src='qrc:/images/spacer.gif' width='1' height='1' border='0' alt=''></img>"
                "</td>"
                "<td width='100%'>"
                    "<table cellpadding='0' cellspacing='0' border='0'>"
                        "<tr>"
                            "<td class='gadgetText'>" );

    if ( !sr.isCr )
    {
        srtab += QString( "<tr><td class='gadgetText' width='15%'>Contact</td><td class='gadgetText' width='85%'>" + sr.cus_firstname.trimmed() + " " + sr.cus_lastname.trimmed() );
               
        
        if ( !sr.cus_title.isEmpty() )
        {
            srtab += QString( + " <i>(" + sr.cus_title.trimmed() + ")</i>" );
        }
        
        if ( !sr.cus_lang.isEmpty() )
        {
            srtab += QString( + " -- " + sr.cus_lang.trimmed() );
        }
        
        if ( !sr.cus_email.isEmpty() )
        {
            srtab += QString( + " -- " + sr.cus_email.trimmed() );
        }
        
        if ( !sr.cus_phone.isEmpty() )
        {
            srtab += QString( + " -- P: " + sr.cus_phone.trimmed() );
        }
        
      srtab += QString( "" 
                         "<tr>"
                            "<td class='gadgetText'>Contact via</td>"
                            "<td class='gadgetText'>" + sr.respond_via + "</td>"
                         "</tr>" );
    }
        srtab += QString( "</td></tr><tr>"
                            "<td class='gadgetText'>Status</td>"
                            "<td class='gadgetText'>" + sr.status + "</td>"
                          "</tr>" );
        
        if ( sr.isChat )
        {
            srtab += QString(   "<tr>"
                                   "<td class='gadgetText'>Bomgar Queue/Owner&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>" + sr.bomgarQ + "</td>"
                                "</tr>" );
        }
        

        
  
        srtab += QString(       "<tr>"
                                   "<td class='gadgetText'>Contract</td>"
                                   "<td class='gadgetText'>" + sr.support_program_long + "</td>"
                                "</tr>" );
                    
     srtab += QString(          "<tr>"
                                   "<td class='gadgetText'>Last Update&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>" + QString::number( secDays( sr.lastupdatesec ) ) + " days ago</td>"
                                "</tr>" );
    
    if ( sr.highvalue )
    {
        srtab += QString(       "<tr>"
                                   "<td class='gadgetText'>High Value&nbsp;&nbsp;&nbsp;</td>"
                                   "<td class='gadgetText'>Yes <img src='qrc:/images/obacht.png'></img></td>"
                                "</tr>" );
    }

    if ( sr.critsit )
    {
        srtab += QString(       "<tr>"
                                    "<td class='gadgetText'>CritSit&nbsp;&nbsp;&nbsp;</td>"
                                    "<td class='gadgetText'>Yes <img src='qrc:/images/obacht.png'></img></td>"
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

QString HTML::statsPageHeader( Statz s )
{
    QString ph;
    QList<Survey> sl = s.surveyList;
    QList<ClosedItem> cl = s.closedList;
    
    ph += styleSheet();
    
    ph += QString( "<div id='Head1'><img src='qrc:/images/gfx_top_in.gif' width='32' height='80' border='0'/></div>"
                   "<div id='Apptitle' style='left:30px; right:12px; top:0px; height: 80px'>"
                   "<a id='AppTitleA' style='text-decoration: none'>"
                   "<div id='AppTitleDiv' class='apptitle1' style='padding-top: 3px; padding-bottom: 3px; width: 100%'>"
                   "<b>Quarterly Statistics</b>"
                   "</div></div></a>"
                   "<div id='logo1' style='z-index:3'>"
                   "<img src='qrc:/images/logo_im_end.gif' width='12' height='80' border='0'></div></div>"
                   "<div id='content'>" );
    
    if ( sl.isEmpty() || cl.isEmpty() )
    {
        return ph + "Updating...";
    }

    ph += csatTableHeader( s.csatRtsPercent, s.csatEngAvg, s.csatSrAvg );
    
    for ( int i = 0; i < sl.size(); ++i ) 
    {   
        ph += csatTable( sl.at( i ) );
    }
    
    ph += closedTableHeader( s.closedSr, s.srTtsAvg );
    
    
    for ( int i = 0; i < cl.size(); ++i ) 
    {   
        ph += closedTable( cl.at( i ) );
    }
    
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

QString HTML::csatTable( Survey i )
{
    QString srtab;
    
    srtab += QString (  "<table id='" + i.id + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
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
                                    "<a href='sr://" + i.id + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'><td class='gadgetHead'>" );
    
    if ( i.rts ) 
    {
        srtab += QString( "<img src='qrc:/images/yay.gif'></img> " );
    }
    else
    {
        srtab += QString( "<img src='qrc:/images/sad.gif'></img> " );
    }
    
    srtab += QString( "<b>SR#" + i.id + "</b> - " + i.bdesc );
                                            
    srtab += QString( "</td></a>"
                      "<td align='right'>"
                                    "<a href='arrow://" + i.id + "'><img src='qrc:/images/ni_gadget_arrow.gif'></img></a><br>"
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
            "<a href='sr://" + i.id + "'><table id='" + i.id + "_body' width='100%' cellspacing='0' cellpadding='0' "
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
                                    "<td class='gadgetText'>" + i.customer + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Resolved to Satisfaction&nbsp;&nbsp;&nbsp;&nbsp;</td>" );
                                    
    if ( i.rts )
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
                                    "<td class='gadgetText'>" + QString::number( i.srsat ) + "</td>"
                                "</tr>"
                                "<tr>"
                                    "<td class='gadgetText'>Engineer Satisfaction</td>"
                                    "<td class='gadgetText'>" + QString::number( i.engsat ) + "</td>"
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

QString HTML::closedTable( ClosedItem ci )
{
    QString srtab;
    
    srtab += QString (  "<table id='" + ci.id + "_head' width='100%' cellspacing='0' cellpadding='0' border='0'>"
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
                                    "<a href='sr://" + ci.id + "'><table width='100%' cellpadding='0' cellspacing='0' border='0'>"
                                        "<tr width='100%'><td class='gadgetHead'><b>SR#" + ci.id + "</b> - " + 
                                        QString::number( ci.tts ) + " days - " + ci.bdesc );
    

      
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
 
    QString d = "d";
    QString h = "h";
    QString m = "m";
    
    if ( days > 0 )
    {
        sec = ( sec - ( days * 86400 ) );
        hours = ( sec / 3600 );
        sec = ( sec - ( hours * 3600 ) );
        minutes = ( sec / 60 );
        
        if ( days == 1 ) d = "d";
        if ( hours == 1 ) h = "h";
        if ( minutes == 1 ) m = "m";
        
        return QString( QString::number( days ) + " " + d + "  " + QString::number( hours ) + " " + h + "  " +
                        QString::number( minutes ) + " " + m );
    }
    else 
    {
        hours = ( sec / 3600 );
        
        if ( hours > 0 )
        {
            sec = ( sec - ( hours * 3600 ) );
            minutes = ( sec / 60 );        
            
            if ( hours == 1 ) h = "h";
            if ( minutes == 1 ) m = "m";
            
            return QString( QString::number( hours ) + " " + h + "  " + QString::number( minutes ) + " " + m );
        }
        else
        {
            minutes = ( sec / 60 );   
            
            if ( minutes == 1 ) m = "m";
            
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
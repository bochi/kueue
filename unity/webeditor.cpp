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

#include "webeditor.h"
#include "settings.h"

#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QTextDocument>
#include <QTextOption>
#include <QTextBlock>
#include <QTextCursor>

WebEditor::WebEditor( QWebElement element, QString sr, bool format, QObject* parent )
          : QObject( parent )
{
    qDebug() << "[WEBEDITOR] Constructing for SR#" + sr;
    
    mHasValue = element.hasAttribute( "value" );
    mElement = element;

    if ( !mHasValue )
    {
        mElement.evaluateJavaScript( "this.innerHTML = this.value;" );    
    }
    
    // set the directory to save the .txt files
    
    QDir dir ( Settings::editorSaveLocation() );

    // under the user defined directory, create a subdirectory for the sr
    
    if ( !dir.exists( Settings::editorSaveLocation() + "/" + sr ) ) 
    {
        dir.mkdir( sr );
    }
    
    dir.cd( sr );

    // the filename is yyyyMMddhhmmss.txt
    
    QFile file( dir.path() + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".txt" );
    mFileName = file.fileName();
    
    // open the file for writing and put the element's text in it
    
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    
    QTextStream out( &file );
    
    QString content;
    
    if ( mHasValue )
    {
        content = mElement.attribute( "value" );
    }
    else
    {
        content = mElement.toInnerXml();
    }

    content.replace( "&lt;", "<" );
    content.replace( "&gt;", ">" );
    content.replace( "&amp;", "&" );
    
    if ( format )
    {
        QTextDocument doc( content );
        
        for ( int i = 0; i < doc.blockCount(); ++i )
        {
            QStringList words = doc.findBlockByNumber( i ).text().split( " " );
            QString txt;
            int le = Settings::replyFormatLineBreak();
            
            for ( int x = 0; x < words.size(); ++x )
            {
                if ( txt.length() + words.at(x).size() < le )
                {
                    txt += words.at( x ) + " ";
                }
                else
                {
                    le = ( txt.size() + Settings::replyFormatLineBreak() );
                    txt += "\n" + words.at(x) + " ";
                }
            }
            
            QStringList lines = txt.split( "\n" );
            
            for ( int x = 0; x < lines.size(); ++x )
            {
                out << "> " + lines.at(x) + "\n";
            }
        }
    }
    else
    {
        out << content;
    }
    
    file.close();
   
    // define the arguments for the editorCommand
    
    QStringList arg;
    arg.append( file.fileName() );
    
    // and finally run the editor
    
    mProcess = new QProcess( this );
    mProcess->startDetached( Settings::editorCommand(), arg );
    
    // watch the file for changes and write the content back to the element
    
    QTimer* timer = new QTimer( this );
    
    connect( timer, SIGNAL( timeout() ), 
             this, SLOT( writeBack() ) );
    
    timer->start( 1000 );
}

WebEditor::~WebEditor()
{
    qDebug() << "[WEBEDITOR] Destroying";
}

void WebEditor::writeBack()
{
    QString text;
    QFile file( mFileName );
   
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    
    QTextStream in( &file );
    
    while ( !in.atEnd() ) 
    {
        text += in.readLine();
        text += "\n";
    }
    
    file.close();
    
    if ( mText != text )
    {
        mText = text;
        
        if ( mHasValue )
        {
            mElement.setAttribute( "value", text );
        }
        else
        {
            mElement.setInnerXml( text );
            mElement.evaluateJavaScript( "this.value=this.innerHTML.replace( /&gt;/g, \">\" )"
                                         "                         .replace( /&lt;/g, \"<\" )"
                                         "                         .replace( /&amp;/g, \"&\" );" );
        }
    }
}

void WebEditor::killYourself()
{
    mProcess->kill();
    delete this;
}

#include "webeditor.moc"

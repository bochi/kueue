/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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

WebEditor::WebEditor( QWebElement element, QString sr, QObject* parent)
          : QObject(parent)
{
    qDebug() << "[WEBEDITOR] Constructing";
    
    mElement = element;
    
    // set the directory to save the .txt files
    
    QDir dir ( Settings::editorSaveLocation() );

    // under the user defined directory, create a subdirectory for the sr
    
    if ( !dir.exists( Settings::editorSaveLocation() + "/" + sr ) ) dir.mkdir( sr );
    dir.cd( sr );

    // the filename is yyyyMMddhhmmss.txt
    
    QFile file( dir.path() + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".txt" );
    mFileName = file.fileName();
    
    // open the file for writing and put the element's text in it
    
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    
    QTextStream out(&file);
    QString content =  mElement.toInnerXml();
    
    if ( content.isEmpty() )
    {
        content = mElement.attribute( "value" );
    }

    content.replace( "&lt;", "<" );
    content.replace( "&gt;", ">" );
    content.replace( "&amp;", "&" );
    
    out << content;
    
    file.close();
   
    // define the arguments for the editorCommand
    
    QStringList arg;
    arg.append( file.fileName() );
    
    // and finally run the editor
    
    mProcess = new QProcess( this );
    mProcess->start( Settings::editorCommand(), arg );
    
    // watch the file for changes and write the content back to the element
    
    QTimer* timer = new QTimer;
    
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
        qDebug() << "[WEBEDITOR] writing back to" << mFileName;
        mText = text;
        mElement.setInnerXml( text );
	mElement.setAttribute( "value", text );
    }
}

void WebEditor::killYourself()
{
    mProcess->kill();
    delete this;
}

#include "webeditor.moc"

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

#include "nsajob.h"
#include "ui/html.h"
#include "config.h"

#include <QDesktopServices>
#include <QDebug>
#include <QProcess>
#include <QtXml>

NSAJob::NSAJob( const QString& scfile, const QString& sc ) : KueueThread()
{
    

    mSupportConfig = sc;
    mSupportConfigFile = scfile;
    qDebug() << "[NSAJOB] Constructing" << mSupportConfig << mSupportConfigFile;
}

NSAJob::~NSAJob()
{
    qDebug() << "[NSAJOB] Destroying";
}

void NSAJob::run()
{
    QString output;
    NSASummaryItem si;
    QFile info( mSupportConfig + "/summary.xml" );
    qDebug() << info.fileName();
    si.archive = mSupportConfigFile;
    
    if ( !info.open( QFile::ReadOnly ) )
    {
        qDebug() << "[NSAJOB] No summary.xml!";
    }
    else
    {
        QDateTime rundatetime;
        QDomDocument xmldoc;
        
        xmldoc.setContent( &info );
        QDomNode node = xmldoc.namedItem( "summary" );
        qDebug() << node.namedItem( "scriptversion" ).toElement().text();
        
        si.scriptversion = node.namedItem( "scriptversion" ).toElement().text();
        si.scriptdate = QDate::fromString( node.namedItem( "scriptdate" ).toElement().text(), "yyyy MM dd" );
        rundatetime.setDate( QDate::fromString( "20" + node.namedItem( "rundate" ).toElement().text(), "yyyyMMdd" ) );
        rundatetime.setTime( QTime::fromString( node.namedItem( "runtime" ).toElement().text(), "hhmm" ) );
        si.rundate = rundatetime;
        si.hostname = node.namedItem( "hostname" ).toElement().text();
        si.arch = node.namedItem( "arch" ).toElement().text();  
        si.kernel = node.namedItem( "kernel" ).toElement().text();  
        si.slesversion = node.namedItem( "sle_version" ).toElement().text();
        si.slestype = node.namedItem( "sle_type" ).toElement().text();  
        si.slessp = node.namedItem( "sles_sp" ).toElement().text();  
        si.slesversion = node.namedItem( "sles_version" ).toElement().text();  
        si.oesversion = node.namedItem( "oes_version" ).toElement().text();
        si.oessp = node.namedItem( "oes_sp" ).toElement().text();
    }
    
    output += HTML::nsaPageHeader( si );

    mNsaDir = QDir( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/nsa" );
    QString perldir = mNsaDir.absolutePath() + "/perl/";
    
    QStringList files;
    QDirIterator dirWalker( perldir, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
 
    while( dirWalker.hasNext() )
    {
        dirWalker.next();
 
        if( dirWalker.fileInfo().completeSuffix() == "pl" )
        {
            files.append( dirWalker.filePath() );
        }
    }
    
    emit threadStarted( "Generating NSA Report...", files.count() );
    
    QList<NSATableItem> successlist;
    QList<NSATableItem> criticallist;
    QList<NSATableItem> warninglist;
    QList<NSATableItem> recommendedlist;
    
    for ( int i = 0; i < files.size(); ++i ) 
    {
        QProcess p;
        QStringList args;
        
        p.setObjectName( files.at( i ) );
        
        args.append( files.at( i ) );
        args.append( "-p");
        args.append( mSupportConfig );
        
        p.setWorkingDirectory( perldir );
        
        p.start( "perl", args );
        
        if (  !p.waitForFinished ( -1 ) )
        {
            return;
        }
        
        emit threadProgress( i, QString::Null() );
    
        QString meta = p.readAllStandardOutput(); 
        QString pat;

        QStringList metaList = meta.split("|");
            
        NSATableItem ti;
        
        for ( int i = 0; i < metaList.size(); ++i ) 
        {
            if ( metaList.at( i ).startsWith( "META_CATEGORY=" ) )
            {
                QString p = metaList.at( i );
                ti.category = p.remove( "META_CATEGORY=" );
            }
            
            if ( metaList.at( i ).startsWith( "OVERALL=" ) )
            {
                QString m = metaList.at( i ).trimmed();
                ti.type = m.remove( "OVERALL=" ).toInt();
            }
            
            if ( metaList.at( i ).startsWith( "PATTERN_ID=" ) )
            {
                QString p = metaList.at( i );
                pat = p.remove( "PATTERN_ID=" ).toInt();
            }
            
            if ( metaList.at( i ).startsWith( "OVERALL_INFO=" ) )
            {
                QString p = metaList.at( i );
                ti.desc = p.remove( "OVERALL_INFO=" );
            }
        }

        QFile file( p.objectName() );

        if ( !file.open( QFile::ReadOnly ) )
        {
            qDebug() << "couldnt open file";
            return;
        }
    
        QTextStream stream ( &file );
        QString line;
        QList<QStringList> linkList;
        
        while( !stream.atEnd() ) 
        {
            line = stream.readLine();
            
            if ( line.startsWith( "#META-DESCRIPTION" ) )
            {
                //ti.desc = line.split( " = " ).at( 1 );
            }
        
            else if ( line.startsWith("#META-LINK-") )
            {
                QStringList l;
                QString t = line.split( " = " ).at( 0 ).trimmed();
                t.remove( "#META-LINK-" );
                l.append( t );
                l.append( line.split( " = " ).at( 1 ).trimmed() );
                
                linkList.append( l );
            }
        }
        
        ti.linkList = linkList;
       
        if ( ti.type == NSATableItem::Success )
        {
            successlist.append( ti );
        }
        if ( ti.type == NSATableItem::Critical )
        {
            criticallist.append( ti );
        }
        else if ( ti.type == NSATableItem::Warning )
        {
            warninglist.append( ti );
        }
        else if ( ti.type == NSATableItem::Recommended )
        {
            recommendedlist.append( ti );
        }

        file.close();
    }
   
    for ( int i = 0; i < successlist.size(); ++i ) 
    {
        mSuccess += HTML::nsaTableItem( successlist.at( i ) );
    }
    
    for ( int i = 0; i < criticallist.size(); ++i ) 
    {
        mCritical += HTML::nsaTableItem( criticallist.at( i ) );
    }
    
    for ( int i = 0; i < warninglist.size(); ++i ) 
    {
        mWarning += HTML::nsaTableItem( warninglist.at( i ) );  
    }
    
    for ( int i = 0; i < recommendedlist.size(); ++i ) 
    {
        mRecommended += HTML::nsaTableItem( recommendedlist.at( i ) );
    }
    
    output += HTML::nsaTableStart( "Critical" );
    output += mCritical;
    output += HTML::nsaTableEnd();
    output += HTML::nsaTableStart( "Warnings" );
    output += mWarning;
    output += HTML::nsaTableEnd();
    output += HTML::nsaTableStart( "Recommended" );
    output += mRecommended;
    output += HTML::nsaTableEnd();
    output += HTML::nsaTableStart( "Success" );
    output += mSuccess;
    output += HTML::nsaTableEnd();
    
    emit finishedReport( output );
    emit threadFinished( this ); 
}

#include "nsajob.moc"

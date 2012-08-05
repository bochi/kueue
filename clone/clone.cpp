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

#include "clone.h"
#include "qstudio.h"
#include "buildrpm.h"
#include "buildappliance.h"
#include "kueue.h"
#include "kueuethreads.h"
#include "archivers/archiveextract.h"
#include "ui/cloneresult.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QToolBar>
#include <QDesktopServices>
#include <QDebug>
#include <QtXml>

Clone::Clone( const QString& sc )
{
    qDebug() << "[STUDIO] Constructing";
    
    mSupportConfig = sc;
    QDir tmpdir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/kueue-studio";
    
    if ( !tmpdir.exists() )
    {
        tmpdir.mkpath( tmpdir.absolutePath() );
    }
    
    ArchiveExtract* x = new ArchiveExtract( mSupportConfig, tmpdir.absolutePath() );
    
    connect( x, SIGNAL( extracted( QString, QString ) ),
             this, SLOT( downloadScript( QString, QString ) ) );
    
    KueueThreads::enqueue( x );
}

Clone::~Clone()
{
    qDebug() << "[STUDIO] Destroying";
}

void Clone::downloadScript( const QString& archive, const QString& dir )
{
    mScDir = dir;
    QEventLoop loop;
    QNetworkReply* r;

    r = Network::getExt( QUrl( "http://kueue.hwlab.suse.de/clone.sh" ) );
        
    connect( r, SIGNAL( finished() ), 
             this, SLOT( scriptDownloadDone() ) );
}

void Clone::scriptDownloadDone()
{
    QNetworkReply* r = qobject_cast< QNetworkReply* >( sender() );
                    
    QFile file( mScDir.absolutePath() + "/clone.sh" );
    
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        return;
    }
    
    file.write( r->readAll() );
    file.close();
   
    BuildRPM* build = new BuildRPM( mScDir.absolutePath() );
    
    connect( build, SIGNAL( success( QString, QString, QStringList ) ), 
             this, SLOT( buildAppliance( QString, QString, QStringList ) ) );
    
    connect( build, SIGNAL( failed( QString ) ), 
             this, SLOT( failed( QString ) ) );
    
    KueueThreads::enqueue( build );
    /*const QString& a = "SLES11SP2";
    const QString& b = "x86_64";
    QStringList c;
    c.append( "lala" );
    
    
    
    buildAppliance( a, b, c );*/
}

void Clone::buildAppliance( const QString& prod, const QString& arch, const QStringList& result )
{
    CloneResult* res = new CloneResult( this, result );
    int reply = res->exec();

    if ( reply == QDialog::Accepted )
    {
        BuildAppliance* build = new BuildAppliance( mScDir.absolutePath(), prod.trimmed(), arch.trimmed() );
        
        connect( build, SIGNAL( vnc( QUrl ) ), 
                this, SIGNAL( vnc( QUrl ) ) );
        
        KueueThreads::enqueue( build );
    }
}

void Clone::cloneDone()
{
}

void Clone::failed( const QString& msg )
{
    QMessageBox* box = new QMessageBox;
    box->setText( msg );
    box->setWindowTitle( "Error cloning system" );
    box->setIcon( QMessageBox::Critical );

    box->exec();

    delete box;
}

#include "clone.moc"

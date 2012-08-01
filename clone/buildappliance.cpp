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

#include "buildappliance.h"
#include "config.h"
#include "settings.h"

#include <QDebug>
#include <QObject>
#include <QProcess>
#include <QStudio>
#include <QTest>

BuildAppliance::BuildAppliance( const QString& sc, const QString& prod, const QString& arch ) : KueueThread()
{
    qDebug() << "[BUILDAPPLIANCE] Constructing";

    mScDir = sc;
    mProduct = prod;
    mArch = arch;
}

BuildAppliance::~BuildAppliance()
{
    qDebug() << "[BUILDAPPLIANCE] Destroying";
}

void BuildAppliance::run()
{
    QStudio* studio = new QStudio( Settings::studioServer(), Settings::studioUser(), Settings::studioApiKey(), true );
    
    emit threadStarted( "Preparing Appliance...", 0 );
    
    QList<TemplateSet> tl = studio->getTemplates();
    int id;
    QString base;
    
    for ( int i = 0; i < tl.size(); ++i ) 
    {   
        TemplateSet tset = tl.at( i );
        QList<Template> tls = tset.templates;
        
        // For SLES11, we use the JeOS template
        // For SLES10 there is no JeOS, so we use the "Server" template.
        
        for ( int x = 0; x < tls.size(); ++x ) 
        {   
            if ( ( mProduct.contains( "SLES11" ) ) &&
                 ( tls.at( x ).basesystem == mProduct ) &&
                 ( tls.at( x ).name.contains( "JeOS" ) ) )
            {
                id = tls.at( x ).id;
                base = tls.at( x ).basesystem;
            }
            else if ( ( mProduct.contains( "SLES10" ) ) &&
                      ( tls.at( x ).basesystem == mProduct ) &&
                      ( tls.at( x ).name.contains( ", Server" ) ) )
            {
                id = tls.at( x ).id;
                base = tls.at( x ).basesystem;
            }
        }       
    }
    
    Appliance a = studio->cloneAppliance( id, "Clone", mArch );
    
    id = a.id;
    
    QDir dir( mScDir + "/rpms" );
    dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
    QStringList filelist = dir.entryList();
    
    for ( int i = 0; i < filelist.size(); ++i ) 
    { 
        RPM r = studio->uploadRPM( base, mScDir + "/rpms/" + filelist.at( i ) );
        qDebug() << "[BUILDAPPLIANCE] Uploaded" << r.filename;
    }
    
    bool ap = studio->addPackage( id, "clone" );
    
    if ( !ap ) return;
    
    bool aur = studio->addUserRepository( id );
    
    int build;
    
    if ( aur )
    {
        build = studio->startApplianceBuild( id );
    }
    
    qDebug() << "[BUILDAPPLIANCE] The build id is" << build;
    
    emit threadStarted( "Building Appliance...", 100 );
    
    BuildStatus bs = studio->getBuildStatus( build );
    
    do
    {
        bs = studio->getBuildStatus( build );
        emit threadProgress( bs.percent );
        QTest::qSleep( 5000 );
    }
    while ( bs.percent < 100 );
    
    Testdrive td = studio->getTestdrive( build );
    
    QUrl url = "vnc://:" + td.vncpassword + "@" + td.vnchost + ":" + td.vncport;
    emit vnc( url );
    
    qDebug() << td.vnchost << td.vncport << td.vncpassword;
    
    delete studio;
    emit threadFinished( this );
}

#include "buildappliance.moc"

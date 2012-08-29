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

BuildAppliance::BuildAppliance( const QString& sc, const QString& prod, const QString& arch, const QString& hostname ) : KueueThread()
{
    qDebug() << "[BUILDAPPLIANCE] Constructing";

    mScDir = sc;
    mProduct = prod;
    mArch = arch;
    mHostName = hostname;
}

BuildAppliance::~BuildAppliance()
{
    qDebug() << "[BUILDAPPLIANCE] Destroying";
}

void BuildAppliance::run()
{
    QStudio* studio = new QStudio( Settings::studioServer(), Settings::studioUser(), Settings::studioApiKey(), Settings::studioDebugEnabled() );
    
    emit threadStarted( "Preparing Appliance...", 0 );
    
    QFile script( mScDir + "/script.sh" );
    
    if ( !script.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "[BUILDAPPLIANCE]" << "Unable to open script file";
    }
            
    QTextStream out( &script );
    
    out << createBuildScript();
    
    script.close();
    
    QList<TemplateSet> tl = studio->getTemplates();
    int id = 0;
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
    
    if ( id == 0 )
    {
        emit failed( "Failed to get a template to clone - exiting" );
        return;
    }
    
    Appliance a = studio->cloneAppliance( id, mHostName + " - Clone", mArch );
    
    id = a.id;
    
    QDir dir( mScDir + "/rpms" );
    dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
    QStringList filelist = dir.entryList();
    
    for ( int i = 0; i < filelist.size(); ++i ) 
    { 
        RPM r = studio->uploadRPM( base, mScDir + "/rpms/" + filelist.at( i ) );
        qDebug() << "[BUILDAPPLIANCE] Uploaded" << r.filename;
    }
        
    OverlayFile of = studio->addOverlayFile( id, mScDir + "/supportconfig.tar.bz2", "/root/supportconfig" );

    if ( of.filename == "supportconfig.tar.bz2" )
    {
        qDebug() << "[BUILDAPPLIANCE] Uploaded supportconfig";     
    }
    else
    {
        qDebug() << "[BUILDAPPLIANCE] supportconfig upload failed";     
    }
    
    if ( studio->addBuildScript( id, createBuildScript() ) )
    {
        qDebug() << "[BUILDAPPLIANCE] Added custom build script";     
    }
    else
    {
        qDebug() << "[BUILDAPPLIANCE] Adding custom build script failed";     
    }
    
    // use the kueue logo as appliance logo :-)
    
    bool logo = studio->setLogo( id, ":/icons/app/kueue128.png" );
    
    if ( logo )
    {
        qDebug() << "[BUILDAPPLIANCE] Uploaded kueue logo";     
    }
    else
    {
        qDebug() << "[BUILDAPPLIANCE] kueue logo upload failed";     
    }

    bool ap = studio->addPackage( id, "clone" );
    
    if ( !ap ) 
    {
        qDebug() << "[BUILDAPPLIANCE] Failed to add the clone package to the appliance - exiting";
        emit failed( "Failed to add the clone RPM - exiting" );
        return;     
    }
    
    bool aur = studio->addUserRepository( id );
    
    int build;
    
    if ( aur )
    {
        build = studio->startApplianceBuild( id );
        qDebug() << "[BUILDAPPLIANCE] The build id is" << build;
    }
    
    if ( build == 0 )
    {
        int t = 0;
        
        do
        {
            t++;
            emit threadProgress( 0, "Trying to get build slot (" + QString::number( t ) + "/5)" );
            build = studio->startApplianceBuild( id );
            QTest::qSleep( 5000 );
        }
        while ( ( t < 5 ) &&
                ( build == 0 ) );
        
        if ( build == 0 )
        {
            emit failed( "Failed to get build slot..." );
            delete studio;
            emit threadFinished( this );
        }
    }
    else if ( build == -1 )
    {
        emit failed( "Can't start build due to errors in appliance configuration" );
        delete studio;
        emit threadFinished( this );
    }
    else
    {
        BuildStatus bs = studio->getBuildStatus( build );
        
        if ( bs.state == "queued" )
        {
            emit threadProgress( 0, "Waiting for build slot..." );
            
            do
            {
                bs = studio->getBuildStatus( build );
                QTest::qSleep( 5000 );
            }
            while ( bs.state == "queued" );
        }

        emit threadNewMaximum( 100 );
        emit threadProgress( 0, "Building Appliance..." );
        
        do
        {
            bs = studio->getBuildStatus( build );
            emit threadProgress( bs.percent, QString::Null() );
            QTest::qSleep( 5000 );
        }
        while ( ( bs.percent < 100 ) && 
                ( bs.state != "failed" ) );
        
        if ( bs.state != "failed" )
        {
            emit finished( build, mHostName );
        }
        else
        {
            emit failed( bs.message );
        }
        
        delete studio;
        emit threadFinished( this );
    }
}

QString BuildAppliance::createBuildScript()
{
    QString script( "#!/bin/bash\n"
                    "\n"
                    ". /studio/profile\n"
                    ". /.kconfig\n"
                    "echo > /etc/motd\n"
                    "echo Welcome to your clone of " + mHostName + ">> /etc/motd\n"
                    "echo >> /etc/motd\n"
                    "echo You can find the supportconfig this system is based on under /root/supportconfig >> /etc/motd\n"
                    "echo >> /etc/motd\n"
                    "echo Also, there is a split version of the configuration files under >> /etc/motd\n"
                    "echo /root/supportconfig/system  >> /etc/motd\n"
                    "echo >> /etc/motd\n"
                    "echo Have a lot of fun... >> /etc/motd\n"
                    "echo >> /etc/motd\n"
                    "echo echo \"Skipping EULA\" > /etc/init.d/suse_studio_firstboot\n"
                    "echo rm /etc/init.d/suse_studio_firstboot >> /etc/init.d/suse_studio_firstboot\n"
                    "chmod +x /etc/init.d/suse_studio_firstboot\n"
                    "exit 0\n" );
    
    return script;
}

#include "buildappliance.moc"

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

#include "virtappliance.h"
#include "settings.h"
#include "ui/vmexistsdialog.h"

#include <QDebug>
#include <QFileInfo> 
#include <QUuid>
#include <QMessageBox>
#include <QSysInfo>

VirtAppliance::VirtAppliance( const QString& vmdk, const QString& vmx )
{
    qDebug() << "[VIRTAPPLIANCE] Constructing" << QSysInfo::WordSize;
    
    QFile v( vmx );
    QString name;
    
    v.open( QFile::ReadOnly );
    
    while ( !v.atEnd() )
    {
        QString line = v.readLine();
        
        if ( line.startsWith( "displayName" ) )
        {
            name = line.remove( "displayName = " ).remove( "\"" ).trimmed();
        }
    }
    
    mVncWidget = new VncWidget( "QEmu - " + name, RemoteView::High, false );
    mThread = new VirtApplianceThread( vmdk, name );
    
    connect( mThread, SIGNAL( vnc( QUrl ) ),
             mVncWidget, SLOT( createVncView( QUrl ) ) );
    
    connect( mThread, SIGNAL( domainExists( QString ) ),
             this, SLOT( domainExists( QString ) ) );
    
    mThread->start();
}

VirtAppliance::~VirtAppliance()
{
    qDebug() << "[VIRTAPPLIANCE] Destroying";
    
    mThread->deleteWorker();
    mThread->quit();
    mThread->wait();
    
    delete mVncWidget;
    delete mThread;
}

void VirtAppliance::setTabId( int id )
{
    mTabId = id;
}

void VirtAppliance::domainExists( const QString& name )
{
    VmExistsDialog* d = new VmExistsDialog( name );
    
    connect( d, SIGNAL( recreate(QString) ),
             mThread, SLOT( recreateDomain(QString) ) );
    
    connect( d, SIGNAL( connectToInstance( QString ) ),
             mThread, SLOT( connectToInstance( QString ) ) );
    
    d->exec();
    
    delete d;
}

/* 
 * 
 * VirtApplianceThread
 * 
 */

VirtApplianceThread::VirtApplianceThread( const QString& vmdk, const QString& name ) : QThread()
{
    mVmdk = vmdk;
    mName = name;
}

VirtApplianceThread::~VirtApplianceThread()
{
}

void VirtApplianceThread::deleteWorker()
{
    delete mWorker;
}

void VirtApplianceThread::run()
{
    mWorker = new VirtApplianceWorker( mVmdk, mName );
    
    connect( mWorker, SIGNAL( vnc( QUrl ) ),
             this, SIGNAL( vnc( QUrl ) ) );
    
    connect( this, SIGNAL( recreateDomainRequested( QString ) ),
             mWorker, SLOT( recreateDomain( QString ) ) );
    
    connect( mWorker, SIGNAL( domainExists( QString ) ),
             this, SIGNAL( domainExists( QString ) ) );
    
    connect( this, SIGNAL( connectToInstanceRequested( QString ) ),
             mWorker, SLOT( connectToInstance( QString ) ) );
    
    mWorker->startSystemQemu();
    
    exec();
}

void VirtApplianceThread::recreateDomain( const QString& name )
{
    emit recreateDomainRequested( name );
}

void VirtApplianceThread::connectToInstance( const QString& name )
{
    emit connectToInstanceRequested( name );
}

/*
 * 
 * VirtApplianceWorker
 * 
 */

VirtApplianceWorker::VirtApplianceWorker( const QString& vmdk, const QString& name ) : QObject()
{
    qDebug() << "[VIRTAPPLIANCEWORKER] Constructing" << this->thread()->currentThreadId();
    
    mVmdk = vmdk;
    mName = name;
    
    mVirt = new QVirt( this, QVirt::QEmu );
}

VirtApplianceWorker::~VirtApplianceWorker()
{
   qDebug() << "[VIRTAPPLIANCEWORKER] Destroying";
}

void VirtApplianceWorker::startSystemQemu()
{
    QString arch;
    
    if ( mVmdk.contains( "x86_64" ) )
    {
        arch = "x86_64";
    }
    else
    {
        arch = "i386";
    }

    int dom = -1;
    
    if ( !mVirt->getDomains( true ).contains( mName ) )
    {
        dom = mVirt->createDomain( createQemuXML( arch ) );
    }
    else
    {
        qDebug() << "[VIRTAPPLIANCE] Domain with name " + mName + " exists";
        //QString p = mVirt->getScreenshot( mName );
        emit domainExists( mName );
    }
    
    if ( dom != -1 )
    {
        int port = mVirt->getVncPort( dom );
        emit vnc( QUrl( "vnc://127.0.0.1:" + QString::number( port ) ) );
    }
}       

void VirtApplianceWorker::startSystemVmwareWS()
{
    QString arch;
    QFileInfo info( mVmdk );
    
    QDir directory( info.absolutePath() );
    directory.setFilter( QDir::Files );
    directory.setSorting( QDir::Name );
    
    directory.setNameFilters( QStringList() << "*.vmx" );
    QStringList vl = directory.entryList();
    
    for ( int i = 0; i < vl.size(); ++i) 
    {
        QFile::rename( vl.at( i ), vl.at( i ) + ".orig" );
    }
    
    if ( mVmdk.contains( "x86_64" ) )
    {
        arch = "x86_64";
    }
    else
    {
        arch = "i386";
    }
        
    int dom = mVirt->createDomain( createVmwareXML( arch ) );
    qDebug() << mVirt->getVncPort( dom );
}

void VirtApplianceWorker::recreateDomain( const QString& name )
{
    if ( mVirt->destroyDomain( name ) )
    {
        startSystemQemu();
    }
}

QString VirtApplianceWorker::createQemuXML( const QString& arch )
{
    QString xml;
    QString cmd = which( "qemu-system-" + arch );
    
    xml += "<domain type='qemu'>\n";
    xml += "  <name>" + mName + "</name>\n";
    xml += "  <uuid>" + QUuid::createUuid().toString().remove("}").remove("{") + "</uuid>\n";
    xml += "  <memory>1048576</memory>\n";
    xml += "  <currentMemory>1048576</currentMemory>\n";
    xml += "  <vcpu>1</vcpu>\n";
    xml += "  <os>\n";
    xml += "    <type arch='" + arch + "'>hvm</type>\n";
    xml += "  </os>\n";
    xml += "  <clock offset='utc'/>\n";
    xml += "  <on_poweroff>destroy</on_poweroff>\n";
    xml += "  <on_reboot>restart</on_reboot>\n";
    xml += "  <on_crash>destroy</on_crash>\n";
    xml += "  <devices>\n";
    xml += "    <emulator>" + cmd + "</emulator>\n";
    xml += "    <disk type='file' device='disk'>\n";
    xml += "      <driver name='qemu' type='vmdk'/>\n";
    xml += "      <source file='" + mVmdk + "'/>\n";
    xml += "      <target dev='hda' bus='ide'/>\n";
    xml += "      <address type='drive' controller='0' bus='0' unit='0'/>\n";
    xml += "    </disk>\n";
    xml += "    <controller type='ide' index='0'/>\n";
    xml += "    <input type='mouse' bus='ps2'/>\n";
    xml += "    <graphics type='vnc' autoport='yes'/>\n";
    xml += "    <video>\n";
    xml += "      <model type='cirrus' vram='9216' heads='1'/>\n";
    xml += "    </video>\n";
    xml += "    <memballoon model='virtio'/>\n";
    xml += "    </devices>\n";
    xml += "</domain>";
    
    return xml;
}

QString VirtApplianceWorker::createVmwareXML( const QString& arch )
{
    QString xml;
    
    xml += "<domain type='vmware'>\n";
    xml += "  <name>" + mName + "</name>\n";
    xml += "  <uuid>" + QUuid::createUuid().toString().remove("}").remove("{") + "</uuid>\n";
    xml += "  <memory>1048576</memory>\n";
    xml += "<vcpu placement='static'>1</vcpu>\n";
    xml += "<os>\n";
    xml += "    <type arch='" + arch + "'>hvm</type>\n";
    xml += "</os>\n";
    xml += "<clock offset='utc'/>\n";
    xml += "<on_poweroff>destroy</on_poweroff>\n";
    xml += "<on_reboot>restart</on_reboot>\n";
    xml += "<on_crash>destroy</on_crash>\n";
    xml += "<devices>\n";
    xml += "    <disk type='file' device='disk'>\n";
    xml += "      <source file='" + mVmdk + "'/>\n";
    xml += "    <target dev='sda' bus='scsi'/>\n";
    xml += "    <address type='drive' controller='0' bus='0' target='0' unit='0'/>\n";
    xml += "    </disk>\n";
    xml += "    <controller type='scsi' index='0' model='lsilogic'/>\n";
    //xml += "    <controller type='ide' index='0'/>\n";
    xml += "    <interface type='bridge'>\n";
    xml += " <source bridge=''/>\n";
    //xml += "    <mac address='generated'/>\n";
    //xml += "    <source bridge=''/>\n";
    xml += "    <model type='e1000'/>\n";
    xml += "    </interface>\n";
    xml += "    <graphics type='vnc' port='5903'/>\n";
    xml += "<video>\n";
    xml += "    <model type='vmvga' vram='4096'/>\n";
    xml += "</video>\n";
    xml += "</devices>\n";
    xml += "</domain>";
    
    return xml;
}

QString VirtApplianceWorker::which( const QString& command )
{
    const QString paths = QString( getenv( "PATH" ) );

    #ifdef IS_WIN32
    QStringList plist = paths.split(";");
    plist.prepend(QCoreApplication::applicationDirPath());
    #else
    QStringList plist = paths.split(":");
    #endif
    
    for ( QStringList::const_iterator it = plist.begin(); it != plist.end(); it++ )
    {
        #ifdef IS_WIN32
        QFileInfo finfo(*it + "/" + command + ".exe");
        #else
        QFileInfo finfo(*it + "/" + command);
        #endif
        
        if ( finfo.isExecutable() )
        {
            return finfo.absoluteFilePath();
        }
    }
    
    return QString::null;
}

void VirtApplianceWorker::connectToInstance( const QString& name )
{
    int port = mVirt->getVncPort( -1, name );
    emit vnc( QUrl( "vnc://127.0.0.1:" + QString::number( port ) ) );
}

#include "virtappliance.moc"

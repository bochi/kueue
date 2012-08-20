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

#include <QDebug>
#include <QFileInfo> 
#include <QUuid>

VirtAppliance::VirtAppliance( const QString& vmdk, const QString& vmx )
{
    qDebug() << "[VIRTAPPLIANCE] Constructing";
    
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
    
    mVncWidget = new VncWidget( "QEmu", RemoteView::High, false );
    mThread = new VirtApplianceThread( vmdk, name );
    
    connect( mThread, SIGNAL( vnc( QUrl ) ),
             mVncWidget, SLOT( createVncView( QUrl ) ) );
}

VirtAppliance::~VirtAppliance()
{
    qDebug() << "[VIRTAPPLIANCE] Destroying";
}

void VirtAppliance::setTabId( int id )
{
    mTabId = id;
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
    
    start();
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
    
    exec();
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
    startSystemQemu();
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
    
    QFileInfo info( mVmdk );
    
    QFile xmlfile( info.absoluteFilePath() + "/" + mName + ".xml" );
    
    if ( !xmlfile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        return;
    }

    QTextStream out( &xmlfile );
    out << createQemuXML( arch );
    xmlfile.close();
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
    xml += "      <source file='" + mVmdk + "/>\n";
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


#include "virtappliance.moc"

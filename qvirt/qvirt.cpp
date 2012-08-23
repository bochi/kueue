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

#include "qvirt.h"

#include <QDebug>
#include <QtXml>
#include <QDateTime>
#include <QDesktopServices>
#include <stdio.h>
#include <stdlib.h>

QVirt::QVirt( QObject* parent, QVirt::VirtType type )
{
    qDebug() << "[VIRT] Constructing, type:" << type;
    bool c;
    
    if ( type == QVirt::QEmu )
    {
        while ( !connectLocalQemu() )
        {
            qDebug() << "[QVIRT] Connecting...";
        }
    }
    else if ( type == QVirt::VMwareWorkstation )
    {
        while( !connectVmwareWorkstation() )
        {
            qDebug() << "[QVIRT] Connecting...";
        }
    }
}

QVirt::~QVirt()
{
}

int QVirt::credTypes[] = 
{
    VIR_CRED_AUTHNAME,
    VIR_CRED_PASSPHRASE
};

virConnectAuth QVirt::auth = 
{
    credTypes,
    sizeof( credTypes ) / sizeof( int ),
    authCallback,
    NULL,
};

int QVirt::authCallback(virConnectCredentialPtr cred, unsigned int ncred, void *cbdata)
{
    int i;
    AuthData *authData = static_cast< AuthData* >( cbdata );

    for ( i = 0; i < ncred ; ++i ) 
    {
        switch ( cred[ i ].type ) 
        {
            case VIR_CRED_AUTHNAME:
                cred[ i ].result = strdup( authData->username );

                if (cred[i].result == NULL) 
                {
                    return -1;
                }

                cred[i].resultlen = strlen(cred[i].result);
            break;

            case VIR_CRED_PASSPHRASE:
                cred[ i ].result = strdup( authData->password );

                if (cred[ i ].result == NULL) 
                {
                    return -1;
                }

                cred[ i ].resultlen = strlen( cred[i].result );
            break;

            default:
                return -1;
        }
    }

    return 0;
}

static char* qstringToChar( const QString& string )
{
    char* cstr;
    std::string str = string.toStdString();
    cstr = new char[ str.size() + 1 ];
    strcpy( cstr, str.c_str() );
    return cstr;
}

bool QVirt::connectVmwareEsx( const QString& host, const QString& user, const QString& pass )
{
    QString url = "esx://" + host + "/?no_verify=1";
    
    mAuthData.username = qstringToChar( user );
    mAuthData.password = qstringToChar( pass );
    auth.cbdata = &mAuthData;
    
    mConnection = virConnectOpenAuth( url.toUtf8(), &auth, 0 );
    
    if ( mConnection == NULL )
    {
        qDebug() << "[QVIRT] Connection to ESX Host" << host << "failed";
        return false;
    }
    
    qDebug() << "[QVIRT] Connected to" << host << getHypervisorInfo();
    return true;
}

bool QVirt::connectVmwareWorkstation()
{
    QString url = "vmwarews:///session";
    mConnection = virConnectOpenAuth( url.toUtf8(), virConnectAuthPtrDefault, 0 );
    
    if ( mConnection == NULL )
    {
        qDebug() << "[QVIRT] Connection to VMware Workstation failed";
        return false;
    }
    
    qDebug() << "[QVIRT] Connected to VMware Workstation" << getHypervisorInfo();
    return true;
}

bool QVirt::connectLocalQemu()
{
    QString url = "qemu:///session";
    
    /*mAuthData.username = qstringToChar( user );
    mAuthData.password = qstringToChar( pass );
    auth.cbdata = &mAuthData;*/
    
    mConnection = virConnectOpenAuth( url.toUtf8(), virConnectAuthPtrDefault, 0 );
    
    if ( mConnection == NULL )
    {
        qDebug() << "[QVIRT] Connection to QEMU failed";
        return false;
    }
    
    qDebug() << "[QVIRT] Connected to QEMU" << getHypervisorInfo();
    return true;
}


QStringList QVirt::listActiveStoragePools()
{
    int numActivePools = 0;
    char **poolNames = NULL;
    
    QStringList out;
    
    numActivePools = virConnectNumOfStoragePools( mConnection );
    
    if ( numActivePools < 0 ) 
    {
        qDebug() << "[QVIRT] Failed to list active storage pools";
        return out;
    }
    
    poolNames = static_cast<char**>( calloc( numActivePools, sizeof( *poolNames ) ) );
    
    if ( ( virConnectListStoragePools( mConnection, poolNames, numActivePools ) ) < 0 ) 
    {
        qDebug() << "[QVIRT] Failed to list active storage pool names";
        free( poolNames );
        return out;
    }
    
    for ( int i = 0; i < numActivePools; i++ ) 
    {
        virStoragePoolPtr pool = virStoragePoolLookupByName( mConnection, poolNames[ i ] );
        
        if ( !pool ) 
        {
            free( poolNames[ i ] );
            continue;
        }
        else
        {
            out.append( poolNames[ i ] );   
        }
    }
    
    return out;
}


QString QVirt::getHypervisorInfo()
{
    QString hv;
    unsigned long hvVer, major, minor, release;
    const char *hvType;

    hvType = virConnectGetType( mConnection );

    if ( hvType == NULL ) 
    {
        hv = "Failed to get hypervisor type"; 
        return hv;
    }

    if ( virConnectGetVersion( mConnection, &hvVer ) != 0 ) 
    {
        hv = "Failed to get hypervisor version";
        return hv;
    }

    major = hvVer / 1000000;
    hvVer %= 1000000;
    minor = hvVer / 1000;
    release = hvVer % 1000;

    hv = QString( hvType ) + " " + QString::number( major ) + "." + QString::number( minor ) + "." + QString::number( release );
    return hv;
}

QStringList QVirt::getInactiveDomains()
{
    int ret = 0, i, numNames, numInactiveDomains;
    char **nameList = NULL;
    QStringList out;

    numInactiveDomains = virConnectNumOfDefinedDomains( mConnection );
    
    if ( numInactiveDomains == -1 ) 
    {
        qDebug() << "[VIRT] Failed to get number of inactive domains";
        return out;
    }

    nameList = static_cast<char**>( malloc( sizeof( *nameList ) * numInactiveDomains ) );

    if ( nameList == NULL ) 
    {
        qDebug() << "[VIRT] Could not allocate memory for list of inactive domains";
        return out;
    }

    numNames = virConnectListDefinedDomains( mConnection, nameList, numInactiveDomains );

    if ( numNames == -1 ) 
    {
        qDebug() << "[VIRT] Could not get list of defined domains from hypervisor";
        return out;
    }

    for ( i = 0 ; i < numNames ; i++ ) 
    {
        out.append( QString( *(nameList + i) ) );
        free( *( nameList + i ) );
    }

    free( nameList );
    return out;
}

QStringList QVirt::getActiveDomains()
{
    int ret = 0, i, numNames, numActiveDomains;
    char **nameList = NULL;
    QStringList out;

    numActiveDomains = virConnectNumOfDomains( mConnection );
    
    if ( numActiveDomains == -1 ) 
    {
        qDebug() << "[VIRT] Failed to get number of active domains";
        return out;
    }

    nameList = static_cast<char**>( malloc( sizeof( *nameList ) * numActiveDomains ) );

    if ( nameList == NULL ) 
    {
        qDebug() << "[VIRT] Could not allocate memory for list of active domains";
        return out;
    }

    numNames = virConnectListDefinedDomains( mConnection, nameList, numActiveDomains );

    if ( numNames == -1 ) 
    {
        qDebug() << "[VIRT] Could not get list of defined domains from hypervisor";
        return out;
    }

    for ( i = 0 ; i < numNames ; i++ ) 
    {
        out.append( QString( *(nameList + i) ) );
        free( *( nameList + i ) );
    }

    free( nameList );
    return out;
}

QStringList QVirt::getDomains( bool active )
{
    virDomainPtr *allDomains;
    int numDomains;
    int numActiveDomains, numInactiveDomains;
    char **inactiveDomains;
    int *activeDomains;
    QStringList out;
    
    numActiveDomains = virConnectNumOfDomains( mConnection );        
    numInactiveDomains = virConnectNumOfDefinedDomains( mConnection );

    allDomains = static_cast<virDomain**>( malloc( sizeof( virDomainPtr ) * numActiveDomains + numInactiveDomains ) );
    inactiveDomains = static_cast<char**>( malloc( sizeof( char * ) * numDomains ) );
    activeDomains = static_cast<int*>( malloc( sizeof( int ) * numDomains ) );

    numActiveDomains = virConnectListDomains( mConnection, activeDomains, numActiveDomains );
    numInactiveDomains = virConnectListDefinedDomains( mConnection, inactiveDomains, numInactiveDomains );

    for ( int i = 0 ; i < numActiveDomains ; i++) 
    {
        out.append( virDomainGetName( virDomainLookupByID( mConnection, activeDomains[i]) ) );
        numDomains++;
    }

    for ( int i = 0 ; i < numInactiveDomains ; i++) 
    {
        if ( !active )
        {
            out.append( virDomainGetName( virDomainLookupByName( mConnection, inactiveDomains[i] ) ) );
        }
        free( inactiveDomains[ i ] );
        numDomains++;
    }
    
    free( activeDomains );
    free( inactiveDomains );
    return out;
}

int QVirt::createDomain( const QString& xml )
{
    virDomainPtr dom;
    const char *from = NULL;
    char *buffer = qstringToChar( xml );
    unsigned int flags = VIR_DOMAIN_NONE;
    int id = -1;
    
    dom = virDomainCreateXML( mConnection, buffer, flags );
    free( buffer );

    if ( dom != NULL ) 
    {
        qDebug() << "[QVIRT] Created domain " + QString( virDomainGetName( dom ) ) + " with ID " + QString::number( virDomainGetID( dom ) );
        id = virDomainGetID( dom );
        virDomainFree( dom );
    }
    else 
    {
        qDebug() << "[QVIRT] Failed to create domain";
    }
    
    return id;
}

int QVirt::getVncPort( int domain, const QString& name )
{
    virDomainPtr dom;
    
    if ( domain != -1 )
    {
        dom = virDomainLookupByID( mConnection, domain );
    }
    else if ( !name.isNull() )
    {
        dom = virDomainLookupByName( mConnection, qstringToChar( name ) );
    }
    
    int port = 0;
    char* doc = NULL;
    char* listen_addr = NULL;
    bool ret = true;

    /* Check if the domain is active and don't rely on -1 for this */
    if ( !virDomainIsActive( dom ) )
    {
        qDebug() << "[QVIRT] Domain " + QString::number( virDomainGetID( dom ) ) + " is not active";
        ret = false;
        port = -1;
    }

    if ( !( doc = virDomainGetXMLDesc( dom, 0 ) ) )
    {
        qDebug() << "[QVIRT] Unable to get XML description for domain " + QString::number( virDomainGetID( dom ) );
        ret = false;
        port = -1;
    }

    if ( ret )
    {
        QString xml = doc;
        QDomDocument xd;
        xd.setContent( xml.trimmed() );
        QDomElement docelement = xd.documentElement();
        
        port = docelement.firstChildElement( "devices" )
                         .firstChildElement( "graphics" )
                         .toElement().attribute( "port" ).toInt();
    }
    
    free( doc );
    free( listen_addr );
    virDomainFree( dom );
    return port;
}

bool QVirt::destroyDomain( const QString& d )
{
    virDomainPtr dom = virDomainLookupByName( mConnection, qstringToChar( d ) );
    bool ret = true;
    
    int result = virDomainDestroy( dom );

    if (result == 0) 
    {
        qDebug() << "[QVIRT] Destroyed domain" << d;
    }
    else 
    {
        qDebug() << "[QVIRT] Failed to destroy domain" << d;
        ret = false;
    }

    virDomainFree( dom );
    return ret;
}

static int mysink(virStreamPtr st, const char *buf, size_t nbytes, void *opaque) 
{ 
    qDebug() << "sink";
  //int *fd = static_cast<int*>(opaque);
  QDataStream* s = static_cast<QDataStream*>(opaque);
  return s->writeRawData( buf, nbytes );
} 

QString QVirt::getScreenshot( const QString& domain )
{
    qDebug() << "getscr";
    virDomainPtr dom;
    QString filename;
    virStreamPtr st;
    int ret = false;
    QString mime;
    QString ext;

    dom = virDomainLookupByName( mConnection, qstringToChar( domain ) );

    st = virStreamNew( mConnection, 0 );
    qDebug() << st;

    mime = virDomainScreenshot( dom, st, 0, 0 );
    qDebug() << mime;
    
    if ( mime == "image/x-portable-pixmap" )
    {
        ext = ".ppm";
    }
    else if ( mime == "image/png" )
    {
        ext = ".png";
    }
    
    QString d = QDateTime::currentDateTime().toString( "yyyyMMdd-hhmmss" );
    QString f = virDomainGetName( dom ) + QString( "-" ) + d + ext;
    QString dir = QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + "/kueue" ;
    filename = dir + "/" + f;
    qDebug() << filename;
    QFile file( filename );
    
    
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "FNO";
        return "bla";
    }
    
    QDataStream stream( &file );

    //int fd = file.handle();
    qDebug() << "vsrcv";
    qDebug() << virStreamRecvAll( st, mysink, &stream );
    
    qDebug() << "vsfin";
    if ( virStreamFinish( st ) < 0 )
    {
        qDebug() << "ERR FINISH";
    }   
    
    file.flush();
    file.close();

    
    virDomainFree(dom);
    virStreamFree(st);
   
    return filename;
}

#include "qvirt.moc"
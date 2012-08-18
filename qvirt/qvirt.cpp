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
#include <stdio.h>
#include <stdlib.h>

QVirt::QVirt( QObject* parent )
{
    qDebug() << "[VIRT] Constructing";
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

char* QVirt::qstringToChar( const QString& string )
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
        qDebug() << "[VIRT] Connection to ESX Host" << host << "failed";
        return false;
    }
    
    qDebug() << "[VIRT] Connected to" << host << getHypervisorInfo();
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
        qDebug() << "[VIRT] Connection to QEMU failed";
        return false;
    }
    
    qDebug() << "[VIRT] Connected to QEMU" << getHypervisorInfo();
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



#include "qvirt.moc"
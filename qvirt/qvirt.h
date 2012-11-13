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

#ifndef QVIRT_H
#define QVIRT_H

#include <QObject>
#include <QStringList>
#include <QPixmap>

extern "C" 
{
    #include <libvirt/libvirt.h>
}

class QVirt : public QObject
{
    Q_OBJECT

    struct _AuthData 
    {
        char *username;
        char *password;
    };
    
    typedef struct _AuthData AuthData;
    
    public:
        
        enum VirtType
        {
            QEmu = 1,
            VMwareWorkstation = 2,
            VMwarePlayer = 3,
            VmwareESX = 4,
            VirtualBox = 5
        };
        
        QVirt( QObject* parent, QVirt::VirtType );
         ~QVirt();
         
        QStringList listActiveStoragePools();

        bool connectVmwareEsx( const QString& host, const QString& user, const QString& pass );
        bool connectLocalQemu();
        bool connectVmwareWorkstation();
        bool destroyDomain( const QString& );
        int createDomain( const QString& );
        int getVncPort( int = -1, const QString& = QString::Null() );
        QStringList getInactiveDomains();
        QStringList getActiveDomains();
        QStringList getDomains( bool = false );
        //QString getScreenshot( const QString& );
        
    private: 
        virConnectPtr mConnection;
        AuthData mAuthData;
        
        //char* qstringToChar( const QString& );
        static virConnectAuth auth;
        static int credTypes[];
        static int authCallback(virConnectCredentialPtr, unsigned int, void* );
        static int showDomains(virConnectPtr conn);
        //static int vshStreamSink(virStreamPtr, const char *bytes, size_t nbytes, void *opaque);
        //virStreamSinkFunc mysink(virStreamPtr st, const char *buf, int nbytes, void *opaque);
        //static char genFileName( virDomainPtr dom, const char *mime )
        //static ssize_t safewrite(int fd, const void *buf, size_t count);
        //int mysink(virStreamPtr st, const char *buf, size_t nbytes, void *opaque) ;
        QString getHypervisorInfo();

};
 
#endif
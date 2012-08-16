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
        QVirt( QObject* parent = 0L );
         ~QVirt();
         
    public slots:
        bool connectVmwareEsx( const QString& host, const QString& user, const QString& pass );
         
    private: 
        virConnectPtr mConnection;
        AuthData mAuthData;
        
        char* qstringToChar( const QString& );
        static virConnectAuth auth;
        static int credTypes[];
        static int authCallback(virConnectCredentialPtr, unsigned int, void* );
        static int showDomains(virConnectPtr conn);
        QString getHypervisorInfo();
        QStringList getInactiveDomains();
        QStringList getActiveDomains();


    
    
};
 
#endif
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

#ifndef QSTUDIO_H
#define QSTUDIO_H

#include <QObject>
#include <QtNetwork>

#include "dataclasses.h"

class QStudio : public QObject
{
    Q_OBJECT

    public: 
        QStudio( const QString&, const QString&, const QString&, bool = false );
        ~QStudio();
        
        QList<TemplateSet> getTemplates();
        Appliance cloneAppliance( int, const QString&, const QString& );
        RPM uploadRPM( const QString&, const QString& );
        bool addUserRepository( int );
        bool addPackage( int, const QString& );
	int startApplianceBuild( int );
        BuildStatus getBuildStatus( int );
        Testdrive getTestdrive( int );
        QList<UserTestDrive> getUserTestdrives();
        OverlayFile addOverlayFile( int, const QString&, const QString&, const QString& = QString::Null(), const QString& = QString::Null(), const QString& = QString::Null() );
        bool setLogo( int, const QString& );
        QString getDownloadUrlForBuild( int );
        bool deleteBuild( int );
        
    private:
        bool mDebug;
        QString mUser;
        QString mApiKey;
        QString mServer;
        QNetworkAccessManager* mNAM;
        QString getRequest( const QString& );
        QString putRequest( const QString&, const QByteArray& );
        QString postRequest( const QString&, const QByteArray& );
        QString postFile( const QString&, const QString& );
        QString putFile( const QString&, const QString& );
        QString deleteRequest( const QString& );
        
    private slots:
        void log( const QString&, const QString& );
        void networkError( QNetworkReply::NetworkError );
        void authenticate( QNetworkReply*, QAuthenticator* );
};

#endif

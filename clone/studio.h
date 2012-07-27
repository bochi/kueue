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

#ifndef STUDIO_H
#define STUDIO_H

#include <QtNetwork>
#include <QtCore/qvarlengtharray.h>

class Template;
class TemplateSet;
class Appliance;

class Studio : public QObject
{
    Q_OBJECT

    public: 
        Studio( const QString& );
        ~Studio();
        
    public:
        QList<TemplateSet> getTemplates();
        Appliance cloneAppliance( int, const QString&, const QString& );
        bool uploadRPM( const QString& );

    private:
        QNetworkAccessManager* mNAM;
        QString getRequest( const QString& );
        QString putRequest( const QString&, const QByteArray& );
        QString postRequest( const QString&, const QByteArray& );
        QString deleteRequest( const QString& );
        
    private slots:

        void networkError( QNetworkReply::NetworkError );
        void authenticate( QNetworkReply*, QAuthenticator* );
};

class TemplateSet
{
    public:
        QString name;
        QString description;
        QList<Template> templates;
};

class Template
{
    public:
        int id;
        QString name;
        QString description;
        QString basesystem;
};

class Appliance
{
    public:
        int id;
        QString name;
        QString arch;
        QString type; 
        QString last_edited;
        QString estimated_raw_size;
        QString estimated_compressed_size;
        QString edit_url;
        QString basesystem;
        QString uuid; 
        int parent;
};

class RPM
{
    public:
        int id;
        QString filename;
        QString size;
        bool archive;
        QString basesystem;
};


#endif

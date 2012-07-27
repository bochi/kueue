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

#include "studio.h"
#include "config.h"
#include "settings.h"

#include <QDebug>
#include <QObject>
#include <QtXml>

Studio::Studio( const QString& sc )
{
    qDebug() << "[STUDIO] Constructing";

    mNAM = new QNetworkAccessManager( this );
    
    connect( mNAM, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ), 
             this, SLOT( authenticate( QNetworkReply*, QAuthenticator* ) ) );
    
    QList<TemplateSet> l = getTemplates();
    
    for ( int i = 0; i < l.size(); ++i ) 
    {   
        TemplateSet tset = l.at( i );
        QList<Template> tls = tset.templates;
        
        for ( int x = 0; x < tls.size(); ++x ) 
        {   
            qDebug() << "TLS" << tls.at( x ).name;
        }
    }
}

Studio::~Studio()
{
    qDebug() << "[STUDIO] Destroying";
}

QString Studio::getRequest( const QString& req )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + Settings::studioServer() + "/api/v2" + req ) );
    QNetworkReply *reply = mNAM->get( request );
    reply->ignoreSslErrors(); 
    qDebug() << request.url();
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    
    return result;
}

QString Studio::putRequest( const QString& req, const QByteArray& data )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + Settings::studioServer() + "/api/v2" + req ) );
    QNetworkReply *reply = mNAM->put( request, data );
     
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    
    return result;
}

QString Studio::postRequest( const QString& req, const QByteArray& data )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + Settings::studioServer() + "/api/v2" + req ) );
    QNetworkReply *reply = mNAM->get( request );
     
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    
    return result;
}

QString Studio::deleteRequest( const QString& req )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + Settings::studioServer() + "/api/v2" + req ) );
    QNetworkReply *reply = mNAM->get( request );
     
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    
    return result;
}

void Studio::authenticate( QNetworkReply* reply, QAuthenticator* auth )
{
    auth->setUser( Settings::studioUser() );
    auth->setPassword( Settings::studioApiKey() );
}

void Studio::networkError( QNetworkReply::NetworkError error )
{
    qDebug() << "NETWORK ERROR" << error;
}

QList<TemplateSet> Studio::getTemplates()
{
    QString xml = getRequest( "/user/template_sets" );
    QList<TemplateSet> list;
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        TemplateSet ts;
        
        if ( e.tagName() == "template_set" )
        {
            QDomNode no = n.firstChild();
        
            while ( !no.isNull() )
            {
                QDomElement te = no.toElement();
                  
                if ( te.tagName() == "template" )
                {
                    Template t;
                    QDomNode tn = no.firstChild();
                
                    while ( !tn.isNull() )
                    {
                        QDomElement ele = tn.toElement();
                        
                        if ( ele.tagName() == "appliance_id" )
                        {
                            t.id = ele.text().toInt();
                        }            
                        else if ( ele.tagName() == "name" )
                        {
                            t.name = ele.text();
                        }            
                        else if ( ele.tagName() == "description" )
                        {
                            t.description = ele.text();
                        }            
                        else if ( ele.tagName() == "basesystem" )
                        {
                            t.basesystem = ele.text();
                        }            
                        
                        tn = tn.nextSibling();
                    }

                    ts.templates.append( t );
                }
                else if ( te.tagName() == "name" )
                {
                    ts.name = te.text();
                }
                else if ( te.tagName() == "description" )
                {
                    ts.description = te.text();
                }
            
                no = no.nextSibling();
            }

            n = n.nextSibling();
        }

        list.append( ts );
    }
    
    return list;
}

Appliance Studio::cloneAppliance( int id, const QString& name, const QString& arch )
{
    QByteArray empty;
    QString xml = postRequest( "/user/appliances?clone_from=" + QString::number( id ) + "&name=" + name + "&arch=" + arch, empty );
    
    Appliance app;
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "id" )
        {
            app.id = e.text().toInt();
        }
        else if ( e.tagName() == "name" )
        {
            app.name = e.text();
        }
        else if ( e.tagName() == "arch" )
        {
            app.arch = e.text();
        }
        else if ( e.tagName() == "type" )
        {
            app.type = e.text();
        }
        else if ( e.tagName() == "last_edited" )
        {
            app.last_edited = e.text();
        }
        else if ( e.tagName() == "estimated_raw_size" )
        {
            app.estimated_raw_size = e.text();
        }
        else if ( e.tagName() == "estimated_compressed_size" )
        {
            app.estimated_compressed_size = e.text();
        }
        else if ( e.tagName() == "edit_url" )
        {
            app.edit_url = e.text();
        }
        else if ( e.tagName() == "basesystem" )
        {
            app.basesystem = e.text();
        }
        else if ( e.tagName() == "uuid" )
        {
            app.uuid = e.text();
        }
        else if ( e.tagName() == "parent" )
        {
            QDomNode tn = n.firstChildElement( "id" );
            app.parent = n.toElement().text().toInt();
        }

        n = n.nextSibling();
    }
    
    return app;
}

RPM Studio::uploadRPM( const QString& basesystem, const QString& filename )
{
    QFile file( filename );
    QByteArray a( file.readAll() );
    
    QString xml = postRequest( basesystem, a  );
}

#include "studio.moc"

/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>

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

#include "unity/unitynetwork.h"
#include "unity/archiveextract.h"
#include "settings.h"
#include "kueue.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>


UnityNetwork::UnityNetwork( QObject* parent )
        : QNetworkAccessManager( parent )
{
    qDebug() << "[UNITYNETWORK] Constructing";
    clearCookieJar();
}

UnityNetwork::~UnityNetwork()
{
    qDebug() << "[UNITYNETWORK] Destroying";
}

void UnityNetwork::clearCookieJar()
{
    QNetworkCookieJar* jar = new QNetworkCookieJar( this );
    setCookieJar( jar );
}
/*QNetworkReply* UnityNetwork::createRequest( QNetworkAccessManager::Operation op, const QNetworkRequest& req, QIODevice* outgoingData )
{
    // set the browser identification header
    
    QNetworkReply* r = createRequest( op, req, outgoingData );
    
    r->request().setRawHeader( "User-Agent", QString( "kueue" + QApplication::applicationVersion() ).toUtf8() );
    return r;
}*/

#include "unitynetwork.moc"

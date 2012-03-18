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

#include "data.h"
#include "data/database.h"
#include "settings.h"
#include "network.h"
#include "kueue.h"
#include "ui/html.h"

#include <QMessageBox>
#include <QMenu>
#include <QWebFrame>
#include <QWidgetAction>
#include <QWebInspector>
#include <QShortcut>
#include <QGridLayout>
#include <QToolButton>
#include <QWebElementCollection>
#include <QtXml>

Data::Data()
{
    qDebug() << "[DATA] Constructing";
    
}

Data::~Data()
{
    qDebug() << "[DATA] Destroying";
}


#include "data.moc"


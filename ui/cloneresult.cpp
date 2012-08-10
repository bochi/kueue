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

#include "cloneresult.h"

#include <QDebug>
#include <QApplication>
#include <QFileDialog>


CloneResult::CloneResult( QObject* parent, const QStringList& resultlist, const QString& dir )
{
    qDebug() << "[CLONERESULT] Constructing";
    
    setupUi( this ); 
    packageList->setRowCount( resultlist.size() );
    mScDir = dir;
    
    connect( addButton, SIGNAL( clicked() ), 
             this, SLOT( addPackages() ) );
    
    for ( int i = 0; i < resultlist.size(); ++i ) 
    {   
        QTableWidgetItem* p = new QTableWidgetItem( resultlist.at( i ).split( " - " ).at( 0 ) );
        QTableWidgetItem* v = new QTableWidgetItem( resultlist.at( i ).split( " - " ).at( 1 ) );
        QTableWidgetItem* ven = new QTableWidgetItem( resultlist.at( i ).split( " - " ).at( 2 ) );
        
        //int row = packageList->rowCount() + 1;
       
        packageList->setItem( i, 0, p );
        packageList->setItem( i, 1, v );
        packageList->setItem( i, 2, ven );
    }
    
    packageList->resizeColumnsToContents();
}

CloneResult::~CloneResult()
{
    qDebug() << "[CLONERESULT] Destroying";
}

void CloneResult::addPackages()
{
    QString filename = QFileDialog::getOpenFileName( this, "Select package", QDir::homePath(), "RPM Packages (*.rpm)" );
    QFile file( filename );
    QString f = QFileInfo( filename ).fileName();
    
    file.copy( mScDir + "/rpms/" + f );
}


#include "cloneresult.moc"

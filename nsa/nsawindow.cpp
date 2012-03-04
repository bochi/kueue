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

#include "nsawindow.h"
#include "settings.h"

#include <QtGui>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>

NSAWindow::NSAWindow( const QString& filename, const QString& html )
{
    qDebug() << "[NSAWINDOW] Constructing";
    
    mHtml = html;
    
    setAttribute( Qt::WA_DeleteOnClose );
    
    QToolBar* t = new QToolBar( this );
    t->setObjectName( "Toolbar" );
    
    QToolButton* back = new QToolButton( t );
    back->setText( "Back" );
    back->setIcon( QIcon(":/icons/menus/back.png") );
    
    connect( back, SIGNAL( clicked( bool ) ),
             this, SLOT( goBack() ) );
    
    QToolButton* save = new QToolButton( t );
    save->setText( "Save" );
    save->setIcon( QIcon(":/icons/menus/save.png") );
    
    connect( save, SIGNAL( clicked( bool ) ),
             this, SLOT( saveReport() ) );
    
    t->addWidget( back );
    t->addSeparator();
    t->addWidget( save );
    
    addToolBar( t );
    
    mView = new QWebView( this );
    
    setCentralWidget( mView );
    setWindowTitle( "NSA Report - " + filename );
        
    mView->page()->settings()->setFontSize( QWebSettings::MinimumFontSize, Settings::minimumFontSize() );
    mView->page()->settings()->setFontSize( QWebSettings::DefaultFontSize, Settings::defaultFontSize() );
    mView->setHtml( mHtml );
    
    setWindowIcon( QIcon( ":/icons/kueue.png" ) );
    
    resize( Settings::nsaWinSize() );
    move( Settings::nsaWinPos() );
    restoreState( Settings::nsaWinState() );
}

NSAWindow::~NSAWindow()
{
    qDebug() << "[NSAWINDOW] Destroying";
    Settings::setNsaWinPos( pos() );
    Settings::setNsaWinState( saveState() );
    Settings::setNsaWinSize( size() );
}

void NSAWindow::closeEvent( QCloseEvent* e )
{
    e->ignore();
    emit done( this );
}

void NSAWindow::goBack()
{
    mView->setHtml( mHtml );
}

void NSAWindow::saveReport()
{
    QString filename = QFileDialog::getSaveFileName( this, tr("Select filename..."), Settings::downloadDirectory() + "/" + "NSAReport.html", "HTML Files (*.html)"  );
    QFile file( filename );
    
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Text ) )
    {
        return;
    }

    QTextStream out( &file );

    out << mHtml;
    file.close();
}

#include "nsawindow.moc"

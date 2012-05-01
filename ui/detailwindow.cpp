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

#include "detailwindow.h"
#include "data/database.h"
#include <data/data.h>
#include "config.h"
#include "kueue.h"
#include "settings.h"

#include <QtGui>
#include <QDesktopServices>
#include <QHttp>

#ifdef QT_HAS_DBUS
    #include <QtDBus/QDBusConnection>
    #include <QtDBus/QDBusConnectionInterface>
#endif

DetailWindow::DetailWindow( QString sr, bool nb )
{
    qDebug() << "[DETAILWINDOW] Constructing";
    
    setupUi( this ); 
        
    mSr = sr;
    
    #ifndef QT_HAS_DBUS
    
        kopeteCheckBox->setVisible( false );
    
    #endif
    
    QShortcut* closesc = new QShortcut( Qt::Key_Escape, this );
    mTransSC = new QShortcut( Qt::Key_F1, this );
   
    connect( closesc, SIGNAL( activated() ),
             this, SLOT( close() ) );
    
    connect( mTransSC, SIGNAL( activated() ),
             this, SLOT( translate() ) );
    
    if ( Database::isCr( mSr ) )
    {
        srLabel->setText( "<font size='+1'><b>Details for CR#" + mSr + "</b></font>" );
    }
    else
    {
        srLabel->setText( "<font size='+1'><b>Details for SR#" + mSr + "</b></font>" );
    }
    
    if ( Database::getBriefDescription( mSr ) == "ERROR" )
    {
        briefDescLabel->setVisible( false );
        briefDescLabel1->setVisible( false );
    }
    else
    {
        briefDescLabel->setText( Database::getBriefDescription( mSr ) );
    }
    
    mDetails = Database::getDetailedDescription( sr );
    
    if ( mDetails == "ERROR" )
    {
        mDetails.clear();
        downloadDetails();
    }
    else
    {
        detailBrowser->setText( mDetails );
        
        if ( Database::isCr( mSr ) )
        {
            customerLabel1->setText( "Created by:" );
            customerLabel->setText( Database::getCreator( mSr ) );
        }
        else
        {
            customerLabel->setText( Database::getCustomer( mSr ) );
        }
        
        statusLabel->setText( Database::getStatus( mSr ) );
    }

    for ( int i = 0; i < Settings::engineerList().size(); ++i ) 
    {
        assignCombo->addItem( Settings::engineerList().at( i ) );
    }
   
    if ( ( nb ) || 
         ( !Settings::qbossFeatures() ) )
    {
        kopeteCheckBox->setVisible( false );
        assignButton->setVisible( false );
        assignCombo->setVisible( false );
        assignLabel->setVisible( false );
    }
    else
    {
        kopeteCheckBox->setVisible( true );
        assignButton->setVisible( true );
        assignCombo->setVisible( true );
        assignLabel->setVisible( true );
        
        QShortcut* assignsc = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_A ), this );
        
        connect( assignsc, SIGNAL( activated() ),
                 this, SLOT( assignSR() ) );
    }
    
    if ( ( !Settings::qbossFeatures() ) && ( !nb ) )
    {
        takeButton->setVisible( true );

        QShortcut* takesc = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ), this );
        
        connect( takesc, SIGNAL( activated() ),
                 this, SLOT( takePressed() ) );
    }
    else
    {
        takeButton->setVisible( false );
    }
    
    kopeteCheckBox->setChecked( Settings::checkKopete() );

    resize( Settings::detWinSize() );
    move( Settings::detWinPos() );
    restoreState( Settings::detWinState() );
    
    setWindowIcon( QIcon( ":/icons/kueue.png" ) );
    
    connect( closeButton, SIGNAL( clicked() ), 
             this, SLOT( close() ) );
    connect( assignButton, SIGNAL( clicked() ), 
             this, SLOT( assignSR() ) );
    connect( takeButton, SIGNAL( clicked() ),
             this, SLOT( takePressed() ) );
}

DetailWindow::~DetailWindow()
{
    qDebug() << "[DETAILWINDOW] Destroying";
    Settings::setDetWinPos( pos() );
    Settings::setDetWinState( saveState() );
    Settings::setDetWinSize( size() );
}

void DetailWindow::closeEvent( QCloseEvent* e )
{
    e->ignore();
    emit done( this );
}

void DetailWindow::downloadDetails()
{
    mDet2 = Network::get( "/stefan.asp?sr=" );
    
    connect( mDet2, SIGNAL( finished() ), 
             this, SLOT( detail2Finished() ) );
}

void DetailWindow::detailFinished()
{
    QString details = QString::fromUtf8( mDet1->readAll() );

    if( mDet1->error() )
    {
        QMessageBox::critical( this, "Error", "Downloading details for SR# " + mSr + " failed." );
        close();
    }
    else if( details.startsWith( "NOT FOUND") )
    {
        QMessageBox::critical( this, "Error", "SR " + mSr + " not found." );
        close();
    }
    else if( details.isEmpty() )
    {
        detailBrowser->setText( "Sorry, no detailed description available." );
    }
    else
    {
        detailBrowser->setText( details );
    }
}

void DetailWindow::detail2Finished()
{
    if( mDet2->error() )
    {
        statusLabel->setVisible( false );
        statusLabel1->setVisible( false );
        customerLabel->setVisible( false );
        customerLabel1->setVisible( false );
    }
    else
    {
        QString details = mDet2->readAll();
        
        if ( details.contains( "|||" ) )
        {
            details.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
        
            customerLabel->setText( details.split("|||").at( 4 ).trimmed() + " (" + details.split("|||").at( 5 ).trimmed() + ")" );
            statusLabel->setText( details.split("|||").at( 3 ).trimmed() );
        }
        else
        {
            statusLabel->setVisible( false );
            statusLabel1->setVisible( false );
            customerLabel->setVisible( false );
            customerLabel1->setVisible( false );
        }
    }
        
    if ( detailBrowser->document()->isEmpty() )
    {
        qDebug() << "[DETAILWINDOW] No detailed description found, trying another URL...";
                
        mDet1 = Network::get( "detailed/" + mSr );
        
        connect( mDet1, SIGNAL( finished() ),
                 this, SLOT( detailFinished() ) );
    }
}

void DetailWindow::takePressed()
{
    mEngineer = Settings::engineer();
    assignNow();
}

void DetailWindow::translate()
{
    disconnect( mTransSC, 0, 0, 0 );
    
    QHttp* http = new QHttp(this);
    QString text = detailBrowser->toPlainText();
    http->setHost("www.google.com");
    
    connect(http, SIGNAL( done( bool ) ), 
            this, SLOT( transaction_done() ) );
    
    QString url = QString("/translate_a/t?client=j&sl=auto&tl=en" );
    
    QByteArray textByteArray("text=");
    textByteArray.append( detailBrowser->toPlainText().toUtf8() ); 
    
    QHttpRequestHeader header = QHttpRequestHeader("POST", url, 1, 1);
    
    header.setValue("Host", "www.google.com");
    header.setValue("User-Agent", "Mozilla/5.0");
    header.setValue("Accept-Encoding", "deflate");
    header.setContentLength(text.length());
    header.setValue("Connection", "Close");
    
    http->request(header,textByteArray);
}

void DetailWindow::transaction_done()
{
    QHttp* http = qobject_cast< QHttp* >( sender() );
    
    QString   replyText = replyText.fromUtf8( http->readAll() ).split("[").at(1).split("]").at(0);
   
    replyText = replyText.replace(QString("\\\""),QString("\""));
    replyText = replyText.replace(QString("\\n"),QString("\n"));
    replyText = replyText.replace(QString("\n "),QString("\n"));
    replyText = replyText.replace(QString("\\x3c"),QString("<"));
    replyText = replyText.replace(QString("\\x3e"),QString(">"));
    replyText = replyText.replace(QString("\\u003c"),QString("<"));
    replyText = replyText.replace(QString("\\u003e"),QString(">"));

    QStringList translatedList;
 
    replyText = replyText.remove( "{" );
    translatedList = replyText.split(QString("}"));
    
    replyText.clear();
    
    for( int i=0; i < translatedList.count(); i++) 
    {
        replyText.append( translatedList.at(i).split( "\",\"" ).at(0) );
    }
    
    replyText = replyText.remove(",\"").remove( 0, 1 ).remove( "trans\":\"" );
    detailBrowser->setText( replyText );
}

void DetailWindow::assignSR() 
{
    if ( assignCombo->currentText().isEmpty() )
    {
        QMessageBox* box = new QMessageBox( this );
        box->setText( "Please select/enter an engineer" );
        box->setWindowTitle( "Assign SR - Error" );
        box->setStandardButtons(  QMessageBox::Ok );
        box->setIcon( QMessageBox::Critical );
    }
    else
    {
        mEngineer = assignCombo->currentText();
        assignNow();
    }
}

void DetailWindow::assignNow()
{
    QMessageBox* box = new QMessageBox( this );
    
    box->setText( "Assign <b>" + mSr + "</b> to " + mEngineer + "?" );
    box->setWindowTitle( "Assign SR" );
    box->setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    box->setDefaultButton( QMessageBox::Yes );
    box->setIcon( QMessageBox::Question );
    
    int reply = box->exec();
    
    if ( reply == QMessageBox::Yes )
    {
        showProgress( assignCombo->currentText().toUpper() );
        
        mAssign = Network::get( QString( "assign/" + mSr + "|" + mEngineer ) );
        
        connect( mAssign, SIGNAL( finished() ), 
                 this, SLOT( assignJobDone() ) );
    }
    
    delete box;
}

void DetailWindow::assignJobDone()
{
    mProgress->close();
    
    if ( mAssign->error() )
    {
        QMessageBox::critical( this, "Error", "Error sending data to Server" );
        qDebug() << "[DETAILWINDOW] Assign error: " << mAssign->errorString();
    }
    
    QString data = mAssign->readAll();
    
    if ( data.startsWith( "ASSIGNED" ) )
        QMessageBox::critical( this, "Error", "SR already assigned to " + data.split( "|" ).at( 2 ) );

    else if ( data.startsWith( "SR_NOT_FOUND" ) )
        QMessageBox::critical( this, "Error", "SR not found");

    else if ( data.startsWith( "NO_SR" ) )
        QMessageBox::critical( this, "Error", "No SR number specified" );

    else if ( data.startsWith( "NO_OWNER" ) )
        QMessageBox::critical( this, "Error", "No assignee specified" );

    else if ( data.startsWith( "FAILURE" ) )
        QMessageBox::critical( this, "Error", "General failure" );

    else if ( data.startsWith( "SUCCESS" ) )
    {
        QMessageBox::information( this, "Done", "SR successfully assigned to " + mEngineer );
     
        if ( kopeteCheckBox->isChecked() ) 
        {
            sendWithKopete();
        }
    }
    
    else QMessageBox::critical( this, "Error", "Unknown reply: " + data );
     
    Kueue::setClipboard( mSr );
}

void DetailWindow::sendWithKopete()
{
    #ifdef QT_HAS_DBUS
    
        /* This function connects to kopete using dbus and sends a message
        to the engineer selected in the combobox (format engineer.novell) */
        
        const QString dbusServiceName = "org.kde.kopete";
        const QString dbusInterfaceName = "org.kde.Kopete";
        const QString dbusPath = "/Kopete";

        QDBusConnectionInterface* interface = QDBusConnection::sessionBus().interface();

        if ( !interface || !interface->isServiceRegistered( dbusServiceName ) ) return;

        QDBusMessage m = QDBusMessage::createMethodCall( dbusServiceName, dbusPath, dbusInterfaceName, "sendMessage" );
        QList<QVariant> args;

        qDebug() << Settings::kopeteText().replace( "$SR", mSr );
        args.append( mEngineer.toLower() + ".novell" );
        args.append( Settings::kopeteText().replace( "$SR", mSr ) );

        m.setArguments( args );

        QDBusMessage replyMsg = QDBusConnection::sessionBus().call( m );

        if ( replyMsg.type() == QDBusMessage::ReplyMessage )
        {
            if ( !replyMsg.arguments().isEmpty() ) return;
        }
        else if ( replyMsg.type() == QDBusMessage::ErrorMessage )
        {
            QMessageBox::critical( this, "Error", "Sending SR# " + mSr + " to " + mEngineer + " via Kopete failed." );
        }
        else QMessageBox::critical( this, "Error", "Unexpected error." );
    
    #endif
}

void DetailWindow::showProgress( const QString& eng )
{
    mProgress = new QProgressDialog( this );
    mProgress->setWindowTitle( "Assigning SR" );
    mProgress->setLabelText( "Assigning SR to " + eng );
    mProgress->setMinimum( 0 );
    mProgress->setMaximum( 0 );
    mProgress->setWindowModality( Qt::WindowModal );
    mProgress->show();
}

#include "detailwindow.moc"

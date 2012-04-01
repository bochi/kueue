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
    
    PLEASE NOTE:
    
    This is all very ugly and hacky, mostly due to the fact that there is no
    API and this has to be workarounded by parsing urls, titles, elements etc.
    
    If somebody knows a better way to achieve similar functionality, I'm
    open for suggestions :-)
*/

#include "kueue.h"
#include "unitypage.h"
#include "ui/statusbar.h"

#include <QWebElementCollection>
#include <QDesktopServices>
#include <QFileDialog>

UnityPage::UnityPage( QObject *parent, QString sr )
        : QWebPage( ( QWidget* ) 0 )
{
    qDebug() << "[UNITYPAGE] Constructing";
    
    if ( sr != QString::Null() )
    {
        mStartSR = sr;
    }
    
    mViewFrame = 0;
    
    mNavReady = false;
    mLoggedIn = false;
    mDontLogin = false;
    mIsNsaReport = false;
    mSetSC = false;
    mSetSS = false;
    mAddNote = false;
    mNoJsConfirm = false;
    mCloseSR = false;
    mSetStatus = false;
    
    mStatusBar = &mStatusBar->getInstance();
    
    mNAM = new UnityNetwork( this );
    setNetworkAccessManager( mNAM );    
    
    setForwardUnsupportedContent( true );
    setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    
    settings()->setFontSize( QWebSettings::MinimumFontSize, Settings::minimumFontSize() );
    settings()->setFontSize( QWebSettings::DefaultFontSize, Settings::defaultFontSize() );
    settings()->setAttribute( QWebSettings::JavaEnabled, false );
    settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
    settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
    settings()->setAttribute( QWebSettings::JavascriptCanCloseWindows, true );
    
    connect( this, SIGNAL( loadFinished( bool ) ),
             this, SLOT( pageLoaded() ) );
    connect( this, SIGNAL( frameCreated(QWebFrame* ) ), 
             this, SLOT( addFrame( QWebFrame* ) ) );
    connect( this, SIGNAL( unsupportedContent( QNetworkReply* ) ),
             this, SLOT( handleUnsupportedContent(QNetworkReply*) ) );
    connect( this, SIGNAL( downloadRequested( QNetworkRequest ) ),
             this, SLOT( download(QNetworkRequest)));
    connect( this, SIGNAL( selectionChanged() ), 
             this, SLOT( selectionToClipboard() ) );
   
    if ( Settings::unityURL().isEmpty() )
    {
        QEventLoop loop;
        QNetworkReply* r;

        r = Network::get( "unityURL" );
        
        QObject::connect( r, SIGNAL( finished() ), 
                            &loop, SLOT( quit() ) );
                                    
        loop.exec();
                    
        QString url = r->readAll().trimmed();
        Settings::setUnityURL( url );
        
        #ifndef IS_WIN32
        sleep( 1 );
        #endif
    }
    
    mainFrame()->load( QUrl( Settings::unityURL() ) );
   
    mTimer = new QTimer( this );
   
    if ( Settings::useIdleTimeout() )
    {
        mTimer->start( Settings::idleTimeoutMinutes() * 60000 );
    }
    
    connect( mTimer, SIGNAL( timeout() ), 
             this, SLOT( antiIdle() ) );
}

UnityPage::~UnityPage()
{
    qDebug() << "[UNITYPAGE] Destroying";
}

void UnityPage::addFrame( QWebFrame* f )
{    
    if ( f->frameName() == "_sweview" ) 
    {
        mViewFrame = f;
        mLoggedIn = true;
        
        connect( mViewFrame, SIGNAL( loadFinished(bool) ),
                 this, SLOT( viewFrameStarted() ) );
    }
    
    // Set the mBarFrame (the one that holds the navigation bar)    
    
    if ( f->frameName() == "_swescrnbar" )
    {
        mBarFrame = f;
      
        connect( mBarFrame, SIGNAL( loadFinished( bool ) ),
                 this, SLOT( getServiceJS() ) );
    }
    
    // Set the appmenu frame (the one with the menu at the top)
    
    if ( f->frameName() == "_sweappmenu" )
    {
        mMenuFrame = f;
    }
    
    // Set the viewBarFrame (the one that holds "show" and "queries" )
    
    if ( f->frameName() == "_sweviewbar" )
    {
        mViewBarFrame = f;

        connect( mViewBarFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( fixQueryBox() ) );
    }
}

void UnityPage::viewFrameStarted()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    if ( !mStartSR.isEmpty() )
    {
        querySR( mStartSR );
        mStartSR.clear();
    }
}

void UnityPage::fixQueryBox()
{     
    // Set the width for the query combobox to 400 px instead of 140px (default)
    // to make Sascha happy ;-)
    
    QWebElement qb = mViewBarFrame->findFirstElement( "*#s_pdq" );
    qb.setStyleProperty( "width", "400px" );
}

void UnityPage::pageLoaded()
{   
    //QWebSettings::globalSettings()->clearMemoryCaches();
    //qDebug() << mainFrame()->url().toString();
        
    if ( ( mainFrame()->url().toString().contains( "start.swe?SWECmd=Start&SWEHo=siebelprd" ) ) ||
         ( mainFrame()->url().toString().contains( "SWECmd=Login&SWEFullRefresh=1&SWEFullRefresh=1" ) ) )
    {
        if ( mainFrame()->findFirstElement( "input#s_swepi_1" ).attribute("id") != "" )
        loginToUnity();
    }

    else if ( mainFrame()->url().toString().contains( "SWECmd=Logoff&SWEService=SWE+Command+Manager" ) )
    {
        mLoggedIn = false;
        loggedOut();
    }
    
    else if ( mainFrame()->toHtml().contains( "The server you are trying to access is either busy" ) )
    {
        emit pageErbertNed();
        mLoggedIn = false;
        loggedOut();
    }
   
    else if ( ( mViewFrame != 0 ) && ( mLoggedIn ) )
    {
        QString title = mViewFrame->findFirstElement( "title" ).toInnerXml();
    
        //qDebug() << "Title:" << title;
        
        if ( title.startsWith( "NSA Report -" ) )
        {
            mIsNsaReport = true;
        }
        else
        {
            mIsNsaReport = false;
            getCurrentSR();
        }
    }
    else if ( mainFrame()->url().toString() == "about:blank" )
    {
        mainFrame()->load( QUrl( Settings::unityURL() ) );
    }
    
    // Reset the anti idle timer
    
    if ( mTimer->isActive() && Settings::useIdleTimeout() )
    {
        mTimer->stop();
	mTimer->start( Settings::idleTimeoutMinutes() * 60000 );
    }
}

void UnityPage::loggedOut()
{
    // Automagically relogin 
    
    mNAM->clearCookieJar();
    mLoggedIn = false;
    mDontLogin = false;
    mainFrame()->load(  QUrl( Settings::unityURL() ) );
}

void UnityPage::loginToUnity()
{
    mLoggedIn = false;
    
    QWebElement nameInput = mainFrame()->findFirstElement( "input#s_swepi_1" );
    QWebElement passInput = mainFrame()->findFirstElement( "input#s_swepi_2" );
    
    nameInput.setAttribute( "value", Settings::engineer() );
    
    if ( Settings::unityPassword().isEmpty() && !mDontLogin )
    {
        mDontLogin = true;
        PasswordPrompt* p = new PasswordPrompt( this );
        
        connect( p, SIGNAL( passwordReceived() ),
                this, SLOT( passwordReceived()) );
        
        p->exec();
        delete p;
    }
 
    passInput.setAttribute( "value", Settings::unityPassword() );
    
    if ( !mDontLogin && !mLoggedIn )
    {
        qDebug() << "[UNITYPAGE] Trying to login to unity...";
        mainFrame()->evaluateJavaScript( "SWEExecuteLogin(document.SWEEntryForm,'/callcentersi_enu/start.swe','_top')" );
    }
}

void UnityPage::passwordReceived()
{
    mDontLogin = false;
    loginToUnity();
}

void UnityPage::antiIdle()
{
    qDebug() << "[UNITYPAGE] Refreshing due to inactivity";
    
    if ( !mCurrentSR.isEmpty() )
    {
        querySR( mCurrentSR );
    }
    else if ( !mLoggedIn )
    {
        loginToUnity();
    }
    else
    {
        goHome();
    }
}

QWebPage* UnityPage::createWindow( QWebPage::WebWindowType type )
{
    UnityBrowser* browser = qobject_cast< UnityBrowser* >( view() );
    QWebPage* webPage = qobject_cast< QWebPage* > ( browser->newWindow() );
    return webPage;
}

void UnityPage::goToService()
{
    mViewFrame->evaluateJavaScript( mServiceJS );
}

void UnityPage::querySR( const QString& sr )
{
    mNoJsConfirm = false;
    
    // navigate to a specific SR 

    if ( mNavReady )
    {
        if ( ( mSetSS ) || 
             ( mSetSC ) || 
             ( mAddNote ) || 
             ( mCloseSR ) ||
             ( mSetStatus ) )
        {
            mPageErbert = true;
        }
        else
        {
            emit pageErbert( "Browsing to SR#" + sr );
        }
            
        mQuerySR = sr;
        
        disconnect( mViewFrame, 0, 0, 0 );
        
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( goHomeFirst() ) );
        
        mBarFrame->evaluateJavaScript( mServiceJS );
    }
    else 
    {
        qDebug() << "[UNITYPAGE] Navigation not yet usable (probably not finished loading)?";
        StatusBar::showMessage( "Not yet logged in, please wait and try again..." );
    }
}

void UnityPage::goHome()
{
    QWebElementCollection fc = mViewFrame->findAllElements( "a" );   
    QString js;
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if ( fc.at(i).toInnerXml() == "Query" )
        {
            js = fc.at( i ).attribute( "href" ).remove( "Javascript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( js );
}

void UnityPage::goHomeFirst()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( doQuery() ) );

    QWebElementCollection fc = mViewFrame->findAllElements( "a" );   
    QString js;
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if ( ( fc.at( i ).toInnerXml() == "Query" ) && ( fc.at( i ).attribute( "tabindex" ).startsWith( "1" ) ) )
        {
            js = fc.at( i ).attribute( "href" ).remove( "Javascript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( js );
}

void UnityPage::doQuery()
{
    // need to make sure all fields are empty first, otherwise it won't work if 
    // a default query is selected and has some fields filled out automatically
    
    disconnect( mViewFrame, 0, 0, 0 );
    
    connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
             this, SLOT( goToActivities() ) );
    
    QWebElementCollection fc = mViewFrame->findAllElements( "*" );   
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if ( fc.at(i).attribute("id").startsWith( "s_2_2" ) )
        {
            fc.at(i).setInnerXml("");
        }
    }
    
    QWebElement sr = mViewFrame->findFirstElement( "input#s_2_2_94_0" );
    sr.setAttribute( "value", mQuerySR );
    
    QWebElementCollection c = mViewFrame->findAllElements( "a" );   
    QString js;
    
    for ( int i = 0; i < c.count(); ++i ) 
    {  
        if ( c.at(i).toInnerXml() == "Go" )
        {
            js = c.at( i ).attribute( "href" ).remove( "Javascript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( js );
}

void UnityPage::goToActivities()
{
    disconnect( mViewFrame, 0, 0, 0 );

    if ( ( mSetSS ) || ( mSetSC ) || ( mAddNote ) )
    {
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( newActivity()) );
    }
    else if ( mCloseSR )
    {
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( closeSrFirst() ) );
    }
    else if ( mSetStatus )
    {
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( setStatusFirst() ) );
    }
    else
    {
        connect( mViewFrame, SIGNAL( loadFinished(bool) ), 
                 this, SLOT( actionDone() ) );
    }
    
    QWebElementCollection fc = mViewFrame->findAllElements( "a" );
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if ( fc.at(i).attribute("href").contains("Service+Request+Detail+View") )
        {
            QString js = fc.at(i).attribute("href").remove( "Javascript:" );
            mViewFrame->evaluateJavaScript( js );
        }
    }
}

void UnityPage::actionDone()
{
    disconnect( mViewFrame, 0, 0, 0 );
    unsetJsConfirm();
    mPageErbert = false;
    emit pageErbertNed();
}

void UnityPage::newActivity()
{
    disconnect( mViewFrame, 0, 0, 0 );

    if ( mSetSS )
    {                
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( setSSfirst() ) );
    }   
    else if ( mSetSC )
    {
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( setScFirst() ) );
    }       
    else if ( mAddNote )
    {
        connect( mViewFrame, SIGNAL( loadFinished( bool ) ), 
                 this, SLOT( addNoteFirst() ) );
    }       
    
    QWebElementCollection fc = mViewFrame->findAllElements( "a" );
    QString newJS;
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if ( fc.at(i).toInnerXml() == "New" )
        {
            newJS = fc.at(i).attribute( "href" ).remove( "JavaScript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( newJS );
}

void UnityPage::getServiceJS()
{
    disconnect( mBarFrame, 0, 0, 0 );
    
    // get the Javascript that can be used to navigate to the "home" (service) tab
    // it contains some kind of session id, thus needs to be retrieved every time
    
    QWebElementCollection fc = mBarFrame->findAllElements( "a" );
    
    for ( int i = 0; i < fc.count(); ++i ) 
    {  
        if( fc.at( i ).attribute( "href" ).contains("SWEScrnCap=Service" ) && 
            !fc.at( i ).attribute( "href" ).contains("SWEScrnCap=Service+Analytics" ) )
        {
            qDebug() << "[UNITYPAGE] Got the Javascript for navigation";
            
            mServiceJS = fc.at( i ).attribute( "href" ).remove( "Javascript:" );
            mNavReady = true;
        }
    }
}

void UnityPage::unsetJsConfirm()
{
    disconnect( mViewFrame, 0, 0, 0 );
    
    mNoJsConfirm = false;
}

static bool contentSniff( const QByteArray &data )
{
    if ( data.contains( "<!doctype" ) || 
         data.contains( "<script" ) || 
         data.contains( "<html" ) || 
         data.contains( "<!--" ) || 
         data.contains( "<head" )|| 
         data.contains( "<iframe" ) || 
         data.contains( "<h1" ) || 
         data.contains( "<div" ) || 
         data.contains( "<font" ) || 
         data.contains( "<table" ) || 
         data.contains( "<a" ) || 
         data.contains( "<style" ) || 
         data.contains( "<title" ) || 
         data.contains( "<b" ) || 
         data.contains( "<body" ) || 
         data.contains( "<br" ) || 
         data.contains( "<p" ) )
       {
            return true;
       }
        
    return false;
}

void UnityPage::handleUnsupportedContent( QNetworkReply* reply )
{
    if ( !reply )
    {
        return;
    }

    QUrl replyUrl = reply->url();

    if ( replyUrl.scheme() == QLatin1String( "abp" ) )
    {
        return;
    }

    switch ( reply->error() ) 
    {
        case QNetworkReply::NoError:
        {
            if ( reply->header( QNetworkRequest::ContentTypeHeader ).isValid() ) 
            {
                QString ddir;
                
                if ( Settings::useSrDirectory() )
                {
                    mStatusBar->addDownloadJob( reply, Settings::downloadDirectory() + "/" + mCurrentSR, false);
                }
                else
                {
                    mStatusBar->addDownloadJob( reply, Settings::downloadDirectory(), true );
                }
                
                return;
            }
            
            break;
        }
        
        case QNetworkReply::ProtocolUnknownError: 
        {
            QSettings settings;
            settings.beginGroup( QLatin1String( "WebView" ) );
            QStringList externalSchemes = settings.value( QLatin1String( "externalSchemes" ) ).toStringList();
        
            if (externalSchemes.contains( replyUrl.scheme() ) ) 
            {
                QDesktopServices::openUrl( replyUrl );
                return;
            }
        
            break;
        }
    
        default:
        {
            break;
        }
    }

    QWebFrame *notFoundFrame = mainFrame();
    
    if ( !notFoundFrame )
    {
        return;
    }

    if ( reply->header( QNetworkRequest::ContentTypeHeader ).toString().isEmpty() ) 
    {
        QByteArray data = reply->readAll();
        
        if ( contentSniff( data ) ) 
        {
            notFoundFrame->setHtml( QLatin1String( data ), replyUrl );
            return;
        }
    }
}

QWebHitTestResult UnityPage::getElementAt( const QPoint& pos )
{
    return mainFrame()->hitTestContent( pos );
}

void UnityPage::javaScriptAlert( QWebFrame* frame, const QString& msg )
{
    if ( mPageErbert )
    {
        emit pageErbertNed();
    }
    
    if ( msg.contains( "Error[0]:The user ID or password" ) && !mDontLogin )
    {
        mDontLogin = true;
        PasswordPrompt* p = new PasswordPrompt( this );
        
        connect( p, SIGNAL( passwordReceived() ),
                this, SLOT( passwordReceived() ) );
        
        p->exec();
        delete p;
    }
    
    // I have no idea why/when this happens - but it sometimes does (even from firefox)
    
    else if ( msg.contains( "Error[0]:The responsibility of user 'SIEBANON'" ) )
    {
        StatusBar::showMessage( "Unity error (SIEBANON) - logging out and back in..." );
        logout();
    }
    
    // Ugh. I just managed to lock my account while testing... Too many failed login
    // attempts :( Anyway, this is the message... 
    
    else if ( msg.contains( "Error[0]:Login is not allowed at certain times or from" ) )
    {
        // Do something here? 
    }
    
    else if ( msg.contains( "Your session timed out because you were idle" ) )
    {
        loggedOut();
    }
   
    else if ( msg.contains( "You already have a web browser accessing the" ) )
    {
        loggedOut();
    }
    else
    {  
        return QWebPage::javaScriptAlert( frame, msg );
    }
    
    if ( mPageErbert )
    {
        emit pageErbert();
    }
}

bool UnityPage::javaScriptConfirm( QWebFrame* frame, const QString& msg )
{
    if ( msg.contains( "You have unsaved changes" ) && mNoJsConfirm )
    {
        return true;
    }

    return QWebPage::javaScriptConfirm( frame, msg );
}

void UnityPage::setElementText( QWebElement element, const QString& text )
{
    element.setInnerXml( text );
}

void UnityPage::getCurrentSR()
{
    // sets mCurrentSR to the SR that is currently displayed 
    
    QString title = mViewFrame->findFirstElement( "title" ).toInnerXml();
    
    if ( ( title == "All Service Requests" ) ||
            ( title == "My Team's Service Requests" ) ||
            ( title == "My Service Requests" ) ||
            ( title == "All Service Request across Organizations" ) )
    {
        mCurrentSR = "";
        emit currentSrChanged( "" );
    }
    else
    {
        bool keep = false;
        QString srnr = mViewFrame->findFirstElement( "span#s_1_1_69_0" ).toInnerXml().trimmed();
        
        if ( !srnr.isEmpty() )
        {
            mCurrentSR = srnr;
            emit currentSrChanged( srnr );
        }
        else
        {
            QWebElementCollection fc = mViewFrame->findAllElements( "*" );
            
            for ( int i = 0; i < fc.count(); ++i ) 
            {  
                if ( fc.at( i ).attribute( "id" ).contains( "LoginName" ) ||
                    fc.at( i ).attribute( "value" ).contains( Settings::engineer().toUpper() ) )
                {
                    QString csr = fc.at( i ).attribute( "value" ).remove( Settings::engineer().toUpper() + ":" );
                    
                    if ( mCurrentSR != csr )
                    {
                        mCurrentSR = csr;
                        emit currentSrChanged( csr );
                    }
                    
                    i = fc.count();
                    keep = true;
                }
                else if ( fc.at( i ).attribute( "id" ).contains( "s_tb_" ) )
                {
                    i = fc.count();
                    keep = true;
                }
            }
            
            if ( ( !keep ) && 
                ( !title.isEmpty() ) && 
                ( title != "Body" ) && 
                ( title != "Service Request Attachments" ) &&
                ( title != "Service Request Activities" ) )
            {
                mCurrentSR = "";
                emit currentSrChanged( "" );
            }
        }
    }
}    

void UnityPage::sendEmail()
{
    // Execute the javascript that opens the email popup window
    // The rest is done in the PopupWindowWebPage class
    
    if ( Kueue::isSrNr( mCurrentSR ) )
    {
        mMenuFrame->evaluateJavaScript( "SWESubmitForm(document.SWEAppMenuForm,s_0,'','')" );
    }
}

void UnityPage::logout()
{
    mMenuFrame->evaluateJavaScript( "SWEClearHistoryGotoURL('/callcentersi_enu/start.swe?SWEMethod=*Browser*+*Logoff*+*+&SWECmd=Logoff&SWEService=SWE+Command+Manager')" );
}

void UnityPage::selectionToClipboard()
{
    if ( !selectedText().isEmpty() )
    {
        Kueue::setClipboard( selectedText() );
    }
}

void UnityPage::saveNsaReport()
{
    QString filename;
    
    if ( ( Settings::useSrDirectory() ) &&
        !( mCurrentSR.isEmpty() ) )
    {
        QDir dir( Settings::downloadDirectory() + "/" + mCurrentSR );
        
        if ( !dir.exists() )
        {
            dir.mkpath( dir.absolutePath() );
        }

        filename = dir.absolutePath() + "/" + mViewFrame->title().remove( "NSA Report - " ) + ".html";
    }
    else
    {
        filename = QFileDialog::getSaveFileName( 0, tr( "Save File" ), 
                                                  Settings::downloadDirectory() + "/" + mViewFrame->title().remove( "NSA Report - " ) + ".html" );
    }
    
    QFile file( filename );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        return;
    }
    
    QTextStream out( &file );
    out << mViewFrame->toHtml();
    StatusBar::showMessage( "NSA Report saved to " + filename );
}

void UnityPage::download( const QNetworkRequest& req )
{
    if ( Settings::useSrDirectory() )
    {
        mStatusBar->addDownloadJob( req, mNAM, Settings::downloadDirectory() + "/" + mCurrentSR, false);
    }
    else
    {
        mStatusBar->addDownloadJob( req, mNAM, Settings::downloadDirectory(), true );
    }
}

void UnityPage::saveCurrentSR()
{
    QWebElementCollection sc = mViewFrame->findAllElements( "a" );
    QString saveJS;
    
    for ( int i = 0; i < sc.count(); ++i ) 
    {  
        if ( ( sc.at(i).toInnerXml() == "Save" ) &&
             ( sc.at(i).attribute( "tabindex" ).startsWith( "19" ) ) )
        {
            saveJS = sc.at(i).attribute( "href" ).remove( "JavaScript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( saveJS );
}

void UnityPage::saveCurrentActivity()
{
    QWebElementCollection sc = mViewFrame->findAllElements( "a" );
    QString saveJS;
    
    for ( int i = 0; i < sc.count(); ++i ) 
    {  
        if ( sc.at(i).attribute( "id" ).contains( "s_2_1_14" ) )
        {
            saveJS = sc.at(i).attribute( "href" ).remove( "JavaScript:" );
        }
    }
    
    mViewFrame->evaluateJavaScript( saveJS );
}

// To reduce filesize and make the whole thing a bit more managable I split up the code in several files
// and include them here:

#include "unitypage_prod.cpp"
#include "unitypage_ss.cpp"
#include "unitypage_sc.cpp"
#include "unitypage_close.cpp"
#include "unitypage_note.cpp"
#include "unitypage_status.cpp"

#include "unitypage.moc"

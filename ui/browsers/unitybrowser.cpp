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

#include "kueue.h"
#include "settings.h"
#include "unity/webeditor.h"
#include "unity/popupwindow.h"
#include "ui/browsers/unitybrowser.h"

#include <QDebug>
#include <QMenu>
#include <QToolButton>
#include <QWidgetAction>
#include <QShortcut>
#include <QDesktopServices>
#include <QWebInspector>
#include <QWebHistory>
#include <QFileDialog>

UnityBrowser::UnityBrowser( QWidget *parent )
        : QWebView( ( QWidget* ) 0 )
{
    qDebug() << "[UNITYBROWSER] Constructing";
    
    if ( Settings::unityEnabled() )
    {
        QWebSettings::globalSettings()->setAttribute( QWebSettings::JavaEnabled, false );
        QWebSettings::globalSettings()->setAttribute( QWebSettings::JavascriptEnabled, true );
        QWebSettings::globalSettings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
        QWebSettings::globalSettings()->setAttribute( QWebSettings::JavascriptCanCloseWindows, true );
        QWebSettings::globalSettings()->setAttribute( QWebSettings::JavascriptCanAccessClipboard, true );
        
        mUnityPage = new UnityPage( this );
        setPage( mUnityPage );
    
        connect( mUnityPage, SIGNAL( linkHovered( QString, QString, QString ) ), 
                 this, SLOT( urlHovered( QString, QString, QString ) ) );
        
        connect( mUnityPage, SIGNAL( currentSrChanged( QString ) ),
                 this, SIGNAL( currentSrChanged( QString ) ) );
        
        QShortcut* sendEmail = new QShortcut( QKeySequence( Qt::Key_F1 ), this );
   
        connect( sendEmail, SIGNAL( activated() ), 
                mUnityPage, SLOT( sendEmail() ) );
        
        QShortcut* logOut = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_L ), this );
    
        connect( logOut, SIGNAL( activated() ), 
                mUnityPage, SLOT( logout() ) );
        
        QShortcut* fileBrowser = new QShortcut( QKeySequence( Qt::Key_F4 ), this );
    
        connect( fileBrowser, SIGNAL( activated() ), 
                this, SLOT( openFileBrowser() ) );
        
        QShortcut* shortcut = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_I ), this );
    
        connect( shortcut, SIGNAL( activated() ), 
                this, SLOT( openWebInspector() ) );
        
        QShortcut* ss = new QShortcut( QKeySequence( Qt::Key_F2 ), this );
    
        connect( ss, SIGNAL( activated() ), 
                mUnityPage, SLOT( setSS() ) );
        
        QShortcut* sc = new QShortcut( QKeySequence( Qt::Key_F3 ), this );
    
        connect( sc, SIGNAL( activated() ), 
                mUnityPage, SLOT( setSC() ) );
    }
}

UnityBrowser::~UnityBrowser()
{
    delete mUnityPage;
    qDebug() << "[UNITYBROWSER] Destroying";
}

void UnityBrowser::openWebInspector()
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    
    QWidget* w = new QWidget;
    QGridLayout* l = new QGridLayout;
    w->setLayout( l );
    QWebInspector* i = new QWebInspector( w );
    l->addWidget( i );
    i->setPage( page() );
    w->setWindowTitle( "Webinspector - qmonbrowser" );
    w->show();
}

void UnityBrowser::querySR( const QString& sr )
{
    qDebug() << "[UNITYBROWSER] Trying to show SR#" << sr << "in Unity";
    mUnityPage->querySR( sr );
}

void UnityBrowser::urlHovered( const QString& url, const QString& title, const QString& text )
{
    // Copy the last hovered URL to mSr
    // Necessary to make the mouseButtonEvents work on links

    mUrl = url;
}

void UnityBrowser::mousePressEvent( QMouseEvent* event )
{
    if ( ( event->button() == 2 ) &&
         ( mUnityPage->loggedIn() ) )
    {
        contextMenu( event, mUnityPage->currentSr() );
        //qDebug() << mUnityPage->getElementText( element.attribute("id") );
    }
      
    if ( ( event->button() == 4 ) && 
         ( mUnityPage->loggedIn() ) &&
         ( mUnityPage->getElementAt( event->pos() ).element().attribute( "type" ) != "input" ) &&
         ( mUnityPage->getElementAt( event->pos() ).element().attribute( "type" ) != "text" )  &&
         ( !isTextArea( mUnityPage->getElementAt( event->pos() ).element() ) ) &&
         ( Kueue::isSrNr( Kueue::getClipboard() ) ) )
    {
        querySR( Kueue::getClipboard() );
    }
    
    return QWebView::mousePressEvent( event );
}

void UnityBrowser::contextMenu( QMouseEvent* event, const QString& id )
{
    // Create a custom context menu plus a widget to hold the actual menu
    // This makes it look more like a KMenu (way more beautiful)
    
    QWebHitTestResult res = mUnityPage->getElementAt( event->pos() );
    QWebElement element = res.element();
    QMap<int, int> map;
    
    QMenu* menu = new QMenu();
            
    QAction* head = new QAction( menu );
    QAction* his = new QAction( "Back", menu );
    QAction* edit = new QAction( "Open in external editor", menu );
    QAction* copy = new QAction( "Copy", menu );
    QAction* paste = new QAction( "Paste", menu );
    QAction* back = new QAction( "Go back to SR", menu );
    QAction* nsa = new QAction( "Save NSA Report", menu );
    QAction* dlimg = new QAction( "Save image...", menu );
    
    QWidgetAction* wa = new QWidgetAction( menu );
    
    connect( his, SIGNAL( triggered(bool)),
             this, SLOT( historyBack()) );
    
    connect( edit, SIGNAL( triggered(bool) ), 
             this, SLOT( openWebEditor() ) );
    
    connect( copy, SIGNAL( triggered( bool ) ), 
             this, SLOT( copyToClipboard() ) );
    
    connect( back, SIGNAL( triggered( bool ) ), 
             this, SLOT( goBackToSr() ) );
    
    connect( nsa, SIGNAL( triggered( bool ) ), 
             mUnityPage, SLOT( saveNsaReport() ) );
    
    connect( dlimg, SIGNAL( triggered( bool ) ),
             this, SLOT( saveImage() ) );
    
    QFont font = head->font();
    font.setBold( true );
    
    QToolButton* titleButton = new QToolButton( this );
    
    wa->setDefaultWidget( titleButton );
    wa->setObjectName( "MENUTITLE" );
    
    titleButton->setDefaultAction( head );
    titleButton->setDown( true );
    titleButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    
    his->setIcon( QIcon( ":/icons/menus/back.png" ) );
    
    head->setFont( font );
    head->setIcon( QIcon( QIcon( ":/icons/kueue.png" ).pixmap( QSize( 16, 16 ) ) ) );
    head->setText( "kueue" );
    
    edit->setData( QString::number( event->pos().x() ) + "||" + QString::number( event->pos().y() ) + "||" + id );
    edit->setIcon( QIcon( ":/icons/menus/toggle.png" ) );
    
    copy->setData( selectedText() );
    copy->setIcon( QIcon( ":/icons/menus/clipboard.png" ) );
    
    back->setData( mUnityPage->currentSr() );
    back->setIcon( QIcon( ":/icons/menus/srdetails.png" ) );
    nsa->setIcon( QIcon( ":/icons/menus/save.png" ) );
    
    dlimg->setData( QString::number( event->pos().x() ) + "||" + QString::number( event->pos().y() ) + "||" + id );
    dlimg->setIcon( QIcon( ":/icons/menus/download.png" ) );
    
    menu->addAction( wa );
    
    if ( mUnityPage->history()->canGoBack() )
    {
        menu->addAction( his );
        menu->addSeparator();
    }
    
    menu->addAction( edit );
    edit->setEnabled( false );
    
    if ( ( element.attribute( "type" ) == "input" ) ||
         ( element.attribute( "type" ) == "text" ) ||
         ( isTextArea( element ) ) )
    {
        edit->setEnabled( true );
    }
    
    menu->addSeparator();
    menu->addAction( copy );

    if ( selectedText().isEmpty() )
    {  
        copy->setEnabled( false );
    }
    
    menu->addSeparator();
    
    if ( !mUnityPage->currentSr().isEmpty() &&
         Kueue::isSrNr( mUnityPage->currentSr() ) )
    {
        menu->addAction( back ), this, SLOT( goBackToSr() );
    }

    if ( mUnityPage->isNsaReport() )
    {
        menu->addAction( nsa ), mUnityPage, SLOT( saveNsaReport() );
    }

    if ( !res.imageUrl().isEmpty() ) 
    {
        menu->addSeparator();
        menu->addAction( dlimg );
    }
    
    if ( isProductField( element ) )
    {
        menu->addMenu( productMenu( menu ) );
    }
    
    menu->exec( event->globalPos() );
    
    delete menu;
}

QMenu* UnityBrowser::productMenu( QMenu* parent )
{
    QMenu* menu = new QMenu( "Set product...", parent );
    
    QMenu* slesmenu = new QMenu( "SUSE Linux Enterprise Server", menu );
          
    slesmenu->addAction( "SUSE Linux Enterprise Server 9", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 9 SP1", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 9 SP2", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 9 SP3", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 9 SP4", this, SLOT( fillOutProduct() ) );
    
    slesmenu->addSeparator();
    
    slesmenu->addAction( "SUSE Linux Enterprise Server 10", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 10 SP1", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 10 SP2", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 10 SP3", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 10 SP4", this, SLOT( fillOutProduct() ) );
    
    slesmenu->addSeparator();
    
    slesmenu->addAction( "SUSE Linux Enterprise Server 11", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 11 SP1", this, SLOT( fillOutProduct() ) );
    slesmenu->addAction( "SUSE Linux Enterprise Server 11 SP2", this, SLOT( fillOutProduct() ) );
    
    QMenu* sledmenu = new QMenu( "SUSE Linux Enterprise Desktop", menu );
          
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 10", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 10 SP1", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 10 SP2", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 10 SP3", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 10 SP4", this, SLOT( fillOutProduct() ) );
    
    sledmenu->addSeparator();
    
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 11", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 11 SP1", this, SLOT( fillOutProduct() ) );
    sledmenu->addAction( "SUSE Linux Enterprise Desktop 11 SP2", this, SLOT( fillOutProduct() ) );
    
    QMenu* oesmenu = new QMenu( "Open Enterprise Server", menu );
    
    oesmenu->addAction( "Open Enterprise Server", this, SLOT( fillOutProduct() ) );
    oesmenu->addAction( "Open Enterprise Server 2.0.1 Support Pack 1", this, SLOT( fillOutProduct() ) );
    oesmenu->addAction( "Open Enterprise Server 2.0.2 Support Pack 2", this, SLOT( fillOutProduct() ) );
    oesmenu->addAction( "Open Enterprise Server 2.0.3 Support Pack 3", this, SLOT( fillOutProduct() ) );
    
    oesmenu->addSeparator();
    
    oesmenu->addAction( "Open Enterprise Server 11 (OES 11)", this, SLOT( fillOutProduct() ) );
    
    menu->addMenu( slesmenu );
    menu->addMenu( sledmenu );
    menu->addMenu( oesmenu );
    
    return menu;
}

void UnityBrowser::fillOutProduct()
{
    QAction* action = qobject_cast<QAction*>( QObject::sender() );

    if ( action->text().startsWith( "SUSE Linux Enterprise Server" ) )
    {
        mUnityPage->fillOutProduct( "SUSE Linux Enterprise Server", action->text() );
    }
    else if ( action->text().startsWith( "SUSE Linux Enterprise Desktop" ) )
    {
        mUnityPage->fillOutProduct( "SUSE Linux Enterprise Desktop", action->text() );
    }
    else if ( action->text().startsWith( "Open Enterprise Server" ) )
    {
        mUnityPage->fillOutProduct( "Open Enterprise Server", action->text() );
    }
}

void UnityBrowser::historyBack()
{
    mUnityPage->triggerAction( QWebPage::Back );
}

void UnityBrowser::contextMenuEvent( QContextMenuEvent* event )
{
    // Ignore the contextMenuEvent
    // This is necessary to make the right button events work
    
    event->ignore();
}

void UnityBrowser::openWebEditor()
{
    QAction* action = qobject_cast<QAction*>( QObject::sender() );
    
    QString data = action->data().toString();

    QPoint p;
    p.setX( data.split("||").at(0).toInt() );
    p.setY( data.split("||").at(1).toInt() );

    WebEditor* w = new WebEditor( mUnityPage->getElementAt( p ).element(), data.split("||").at( 2 ) );

    connect( mUnityPage, SIGNAL( loadStarted() ),
             w, SLOT( killYourself() ) );
}

void UnityBrowser::copyToClipboard()
{
    QAction* action = qobject_cast<QAction*>( QObject::sender() );
    
    Kueue::setClipboard( action->data().toString() );
}

void UnityBrowser::goBackToSr()
{
    QAction* action = qobject_cast<QAction*>( QObject::sender() );
    
    mUnityPage->querySR( action->data().toString() );
}

void UnityBrowser::saveImage()
{
    QAction* action = qobject_cast<QAction*>( QObject::sender() );
    QString data = action->data().toString();

    QPoint p;
    p.setX( data.split("||").at(0).toInt() );
    p.setY( data.split("||").at(1).toInt() );
    
    QPixmap pix = mUnityPage->viewFrame()->hitTestContent(p).pixmap();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select filename..."), Settings::downloadDirectory() + "/" + mUnityPage->currentSr() + ".jpg" );
    pix.save(fileName);
}

bool UnityBrowser::isTextArea( QWebElement element )
{
    QWebElementCollection c = mUnityPage->viewFrame()->findAllElements( "textarea" );
    
    for ( int i = 0; i < c.count(); ++i )
    {
        if ( c.at( i ) == element )
        {
            return true;
        }
    }
    
    return false;
}

bool UnityBrowser::isProductField( QWebElement element )
{
    if ( ( ( element.attribute("tabindex") == "1024" ) && 
           ( element.attribute("onchange").startsWith("trackChange") ) ) ||
         ( ( element.attribute("tabindex") == "1025" ) && 
           ( element.attribute("onchange").startsWith("trackChange") ) ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

QWebPage* UnityBrowser::newWindow()
{
    PopupWindow* p = new PopupWindow( page()->networkAccessManager(), this );
    return p->webPage();
}

void UnityBrowser::openFileBrowser()
{
    QDesktopServices::openUrl( "file://" + Settings::downloadDirectory() + "/" + mUnityPage->currentSr() );
}

void UnityBrowser::openSearch()
{
    if ( page()->selectedText().isEmpty() )
    {
        emit searchRequested( QString::null );
    }
    else
    {
        emit searchRequested( page()->selectedText() );
    }
}

QString UnityBrowser::currentSR()
{
    return mUnityPage->currentSr();
}

/* 
 * 
 *      UnityWidget class
 * 
 */

UnityWidget::UnityWidget( QObject* parent )
{
    qDebug() << "[UNITYWIDGET] Constructing";
    
    mToolBar = new QToolBar( this );
    mToolBar->setIconSize( QSize( 12, 12 ) );
    
    QGridLayout* unityBrowserLayout = new QGridLayout();
    mUnityBrowser = new UnityBrowser( this );
    
    connect( mUnityBrowser, SIGNAL( currentSrChanged( QString ) ),
             this, SLOT( currentSrChanged( QString ) ) );
    
    WebViewWithSearch* mWebViewWithSearch = new WebViewWithSearch( mUnityBrowser, this );
    setLayout( unityBrowserLayout );

    QToolButton* back = new QToolButton;
    back->setIcon(QIcon( ":/icons/menus/back.png" ) );
    
    QToolButton* backToSR = new QToolButton;
    backToSR->setIcon(QIcon( ":/icons/menus/srdetails.png" ) );
    
    QToolButton* queryGo = new QToolButton;
    queryGo->setIcon(QIcon( ":/icons/menus/ok.png" ) );
    
    
    mSrButton = new QToolButton;
    mSrButton->setEnabled(false);
    mSrButton->setText( "SR" );
    
    connect( back, SIGNAL( pressed() ),
             mUnityBrowser, SLOT( historyBack() ) );
    
    
    connect( backToSR, SIGNAL( pressed() ),
             mUnityBrowser, SLOT( historyBack() ) );
    
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLineEdit* line = new QLineEdit;
    line->setMaximumWidth( 150 );
    
    //line->setFont();
    QLabel* label = new QLabel;
    label->setText( "Query" );
    
    mToolBar->addWidget( back );
    mToolBar->addWidget( backToSR );
    mToolBar->addSeparator();
    mToolBar->addWidget( mSrButton );
    mToolBar->addWidget( spacer );
    mToolBar->addWidget( label );
    mToolBar->addWidget( line );
    mToolBar->addWidget( queryGo );
    
    //unityBrowserLayout->addWidget( mToolBar );
    unityBrowserLayout->addWidget( mWebViewWithSearch );
}

UnityWidget::~UnityWidget()
{
    qDebug() << "[UNITYWIDGET] Destroying id" << mTabId;
}

void UnityWidget::setTabId( int id )
{
    mTabId = id;
}

void UnityWidget::currentSrChanged( QString sr )
{
    mSrButton->setText( "SR#" + sr );
    
    if ( sr == "" )
    {
        mSrButton->setEnabled( false );
    }
    else
    {
        mSrButton->setEnabled( true );
    }
    
}


#include "unitybrowser.moc"

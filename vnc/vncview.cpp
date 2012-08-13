/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
          
                   This code was taken from krdc
         Copyright (C) 2007-2012 Urs Wolfer <uwolfer@kde.org>
          
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

#include "vncview.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QInputDialog>
#define KMessageBox QMessageBox
//#define error(parent, message, caption) \
//  critical(parent, caption, message)

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>

// Definition of key modifier mask constants
#define KMOD_Alt_R 	0x01
#define KMOD_Alt_L 	0x02
#define KMOD_Meta_L 	0x04
#define KMOD_Control_L 	0x08
#define KMOD_Shift_L	0x10

VncView::VncView(QWidget *parent, const KUrl &url, KConfigGroup configGroup)
        : RemoteView(parent),
        m_initDone(false),
        m_buttonMask(0),
        m_repaint(false),
        m_quitFlag(false),
        m_firstPasswordTry(true),
        m_dontSendClipboard(false),
        m_horizontalFactor(1.0),
        m_verticalFactor(1.0),
        m_forceLocalCursor(false)
{
    m_url = url;
    m_host = url.host();
    m_port = url.port();

    connect(&vncThread, SIGNAL(imageUpdated(int,int,int,int)), this, SLOT(updateImage(int,int,int,int)), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(gotCut(QString)), this, SLOT(setCut(QString)), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(passwordRequest()), this, SLOT(requestPassword()), Qt::BlockingQueuedConnection);
    connect(&vncThread, SIGNAL(outputErrorMessage(QString)), this, SLOT(outputErrorMessage(QString)));

    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    
    Q_UNUSED(configGroup);
}

VncView::~VncView()
{
    startQuitting();
}

bool VncView::eventFilter(QObject *obj, QEvent *event)
{
    if (m_viewOnly) {
        if (event->type() == QEvent::KeyPress ||
                event->type() == QEvent::KeyRelease ||
                event->type() == QEvent::MouseButtonDblClick ||
                event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::MouseButtonRelease ||
                event->type() == QEvent::Wheel ||
                event->type() == QEvent::MouseMove)
            return true;
    }

    return RemoteView::eventFilter(obj, event);
}

QSize VncView::framebufferSize()
{
    return m_frame.size();
}

QSize VncView::sizeHint() const
{
    return size();
}

QSize VncView::minimumSizeHint() const
{
    return size();
}

void VncView::setTabID( int id )
{
    mTabID = id;
}

void VncView::scaleResize(int w, int h)
{
    RemoteView::scaleResize(w, h);
    
    //kDebug(5011) << w << h;
    if (m_scale) {
        m_verticalFactor = (qreal) h / m_frame.height();
        m_horizontalFactor = (qreal) w / m_frame.width();

        m_verticalFactor = m_horizontalFactor = qMin(m_verticalFactor, m_horizontalFactor);

        const qreal newW = m_frame.width() * m_horizontalFactor;
        const qreal newH = m_frame.height() * m_verticalFactor;
        setMaximumSize(newW, newH); //This is a hack to force Qt to center the view in the scroll area
        resize(newW, newH);
    } 
}

void VncView::updateConfiguration()
{
    RemoteView::updateConfiguration();

    // Update the scaling mode in case KeepAspectRatio changed
    scaleResize(parentWidget()->width(), parentWidget()->height());
}

void VncView::startQuitting()
{
    //kDebug(5011) << "about to quit";

    setStatus(Disconnecting);

    m_quitFlag = true;

    vncThread.stop();

    unpressModifiers();

    // Disconnect all signals so that we don't get any more callbacks from the client thread
    bool imageUpdatedDisconnect = disconnect(&vncThread, SIGNAL(imageUpdated(int,int,int,int)), this, SLOT(updateImage(int,int,int,int)));
    bool gotCutDisconnect = disconnect(&vncThread, SIGNAL(gotCut(QString)), this, SLOT(setCut(QString)));
    bool passwordRequestDisconnect = disconnect(&vncThread, SIGNAL(passwordRequest()), this, SLOT(requestPassword()));
    bool outputErrorMessageDisconnect = disconnect(&vncThread, SIGNAL(outputErrorMessage(QString)), this, SLOT(outputErrorMessage(QString)));

    //kDebug(5011) << "Signals disconnected: imageUpdated: " << imageUpdatedDisconnect << "gotCut: " << gotCutDisconnect << "passwordRequest: " << passwordRequestDisconnect << "outputErrorMessage: " << outputErrorMessageDisconnect;

    vncThread.quit();

    const bool quitSuccess = vncThread.wait(1000);

    //kDebug(5011) << "Quit VNC thread success:" << quitSuccess;

    setStatus( Disconnected );
}

bool VncView::isQuitting()
{
    return m_quitFlag;
}

bool VncView::start()
{
    vncThread.setHost(m_host);
    vncThread.setPort(m_port);
    RemoteView::Quality quality;

    quality = (RemoteView::Quality)((QCoreApplication::arguments().count() > 2) ?
        QCoreApplication::arguments().at(2).toInt() : 2);

    quality = RemoteView::Quality(RemoteView::High);
    vncThread.setQuality(quality);

    // set local cursor on by default because low quality mostly means slow internet connection
    if (quality == RemoteView::Low) {
        showDotCursor(RemoteView::CursorOn);
    }

    setStatus(Connecting);

    vncThread.start();
    return true;
}

bool VncView::supportsScaling() const
{
    return true;
}

bool VncView::supportsLocalCursor() const
{
    return true;
}

void VncView::requestPassword()
{
    //kDebug(5011) << "request password";

    setStatus(Authenticating);

    if (m_firstPasswordTry && !m_url.password().isNull()) {
        vncThread.setPassword(m_url.password());
        m_firstPasswordTry = false;
        return;
    }

    bool ok;
    QString password = QInputDialog::getText(this, //krazy:exclude=qclasses
                                             tr("Password required"),
                                             tr("Please enter the password for the remote desktop:"),
                                             QLineEdit::Password, QString(), &ok);
    m_firstPasswordTry = false;
    if (ok)
        vncThread.setPassword(password);
    else
        startQuitting();
}

void VncView::outputErrorMessage(const QString &message)
{
    //kDebug(5011) << message;

    if (message == "INTERNAL:APPLE_VNC_COMPATIBILTY") {
        setCursor(localDotCursor());
        m_forceLocalCursor = true;
        return;
    }

    startQuitting();

    emit errorMessage(i18n("VNC failure"), message);
}

void VncView::updateImage(int x, int y, int w, int h)
{
    //kDebug(5011) << "got update" << width() << height();

    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    if (m_horizontalFactor != 1.0 || m_verticalFactor != 1.0) {
        // If the view is scaled, grow the update rectangle to avoid artifacts
        m_x-=1;
        m_y-=1;
        m_w+=2;
        m_h+=2;
    }

    m_frame = vncThread.image();

    if (!m_initDone) {
        setAttribute(Qt::WA_StaticContents);
        setAttribute(Qt::WA_OpaquePaintEvent);
        installEventFilter(this);

        setCursor(((m_dotCursorState == CursorOn) || m_forceLocalCursor) ? localDotCursor() : Qt::BlankCursor);

        setMouseTracking(true); // get mouse events even when there is no mousebutton pressed
        setFocusPolicy(Qt::WheelFocus);
        setStatus(Connected);
        emit connected();
        
        if (m_scale) {
            //TODO: qtonly alternative
        }

        m_initDone = true;

    }

    if ((y == 0 && x == 0) && (m_frame.size() != size())) {
        //kDebug(5011) << "Updating framebuffer size";
        if (m_scale) {
            setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
            if (parentWidget())
                scaleResize(parentWidget()->width(), parentWidget()->height());
        } else {
            //kDebug(5011) << "Resizing: " << m_frame.width() << m_frame.height();
            resize(m_frame.width(), m_frame.height());
            setMaximumSize(m_frame.width(), m_frame.height()); //This is a hack to force Qt to center the view in the scroll area
            setMinimumSize(m_frame.width(), m_frame.height());
            emit framebufferSizeChanged(m_frame.width(), m_frame.height());
        }
    }

    m_repaint = true;
    repaint(qRound(m_x * m_horizontalFactor), qRound(m_y * m_verticalFactor), qRound(m_w * m_horizontalFactor), qRound(m_h * m_verticalFactor));
    m_repaint = false;
}

void VncView::setViewOnly(bool viewOnly)
{
    RemoteView::setViewOnly(viewOnly);

    m_dontSendClipboard = viewOnly;

    if (viewOnly)
        setCursor(Qt::ArrowCursor);
    else
        setCursor(m_dotCursorState == CursorOn ? localDotCursor() : Qt::BlankCursor);
}

void VncView::showDotCursor(DotCursorState state)
{
    RemoteView::showDotCursor(state);

    setCursor(state == CursorOn ? localDotCursor() : Qt::BlankCursor);
}

void VncView::enableScaling(bool scale)
{
    RemoteView::enableScaling(scale);

    if (scale) {
        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        setMinimumSize(1, 1);
        if (parentWidget())
            scaleResize(parentWidget()->width(), parentWidget()->height());
    } else {
        m_verticalFactor = 1.0;
        m_horizontalFactor = 1.0;

        setMaximumSize(m_frame.width(), m_frame.height()); //This is a hack to force Qt to center the view in the scroll area
        setMinimumSize(m_frame.width(), m_frame.height());
        resize(m_frame.width(), m_frame.height());
    }
}

void VncView::setCut(const QString &text)
{
    m_dontSendClipboard = true;
    m_clipboard->setText(text, QClipboard::Clipboard);
    m_dontSendClipboard = false;
}

void VncView::paintEvent(QPaintEvent *event)
{
//     kDebug(5011) << "paint event: x: " << m_x << ", y: " << m_y << ", w: " << m_w << ", h: " << m_h;
    if (m_frame.isNull() || m_frame.format() == QImage::Format_Invalid) {
        kDebug(5011) << "[VNCVIEW] No valid image to paint.";
        RemoteView::paintEvent(event);
        emit somethingWentWrong();
        return;
    }

    event->accept();

    QPainter painter(this);

    if (m_repaint) {
        //kDebug(5011) << "normal repaint";
        painter.drawImage(QRect(qRound(m_x*m_horizontalFactor), qRound(m_y*m_verticalFactor),
                                qRound(m_w*m_horizontalFactor), qRound(m_h*m_verticalFactor)), 
                          m_frame.copy(m_x, m_y, m_w, m_h).scaled(qRound(m_w*m_horizontalFactor), 
                                                                  qRound(m_h*m_verticalFactor),
                                                                  Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
       //kDebug(5011) << "resize repaint";
        QRect rect = event->rect();
        if (rect.width() != width() || rect.height() != height()) {
            //kDebug(5011) << "Partial repaint";
            const int sx = rect.x()/m_horizontalFactor;
            const int sy = rect.y()/m_verticalFactor;
            const int sw = rect.width()/m_horizontalFactor;
            const int sh = rect.height()/m_verticalFactor;
            painter.drawImage(rect, 
                              m_frame.copy(sx, sy, sw, sh).scaled(rect.width(), rect.height(),
                                                                  Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        } else {
             //kDebug(5011) << "Full repaint" << width() << height() << m_frame.width() << m_frame.height();
            painter.drawImage(QRect(0, 0, width(), height()), 
                              m_frame.scaled(m_frame.width() * m_horizontalFactor, m_frame.height() * m_verticalFactor,
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }

    RemoteView::paintEvent(event);
}

void VncView::resizeEvent(QResizeEvent *event)
{
    RemoteView::resizeEvent(event);
    update();
}

bool VncView::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
//         kDebug(5011) << "keyEvent";
        keyEventHandler(static_cast<QKeyEvent*>(event));
        return true;
        break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
//         kDebug(5011) << "mouseEvent";
        mouseEventHandler(static_cast<QMouseEvent*>(event));
        return true;
        break;
    case QEvent::Wheel:
//         kDebug(5011) << "wheelEvent";
        wheelEventHandler(static_cast<QWheelEvent*>(event));
        return true;
        break;
    default:
        return RemoteView::event(event);
    }
}

void VncView::mouseEventHandler(QMouseEvent *e)
{
    if (e->type() != QEvent::MouseMove) {
        if ((e->type() == QEvent::MouseButtonPress) ||
                (e->type() == QEvent::MouseButtonDblClick)) {
            if (e->button() & Qt::LeftButton)
                m_buttonMask |= 0x01;
            if (e->button() & Qt::MidButton)
                m_buttonMask |= 0x02;
            if (e->button() & Qt::RightButton)
                m_buttonMask |= 0x04;
        } else if (e->type() == QEvent::MouseButtonRelease) {
            if (e->button() & Qt::LeftButton)
                m_buttonMask &= 0xfe;
            if (e->button() & Qt::MidButton)
                m_buttonMask &= 0xfd;
            if (e->button() & Qt::RightButton)
                m_buttonMask &= 0xfb;
        }
    }

    vncThread.mouseEvent(qRound(e->x() / m_horizontalFactor), qRound(e->y() / m_verticalFactor), m_buttonMask);
}

void VncView::wheelEventHandler(QWheelEvent *event)
{
    int eb = 0;
    if (event->delta() < 0)
        eb |= 0x10;
    else
        eb |= 0x8;

    const int x = qRound(event->x() / m_horizontalFactor);
    const int y = qRound(event->y() / m_verticalFactor);

    vncThread.mouseEvent(x, y, eb | m_buttonMask);
    vncThread.mouseEvent(x, y, m_buttonMask);
}

void VncView::keyEventHandler(QKeyEvent *e)
{
    // strip away autorepeating KeyRelease; see bug #206598
    if (e->isAutoRepeat() && (e->type() == QEvent::KeyRelease))
        return;

// parts of this code are based on http://italc.sourcearchive.com/documentation/1.0.9.1/vncview_8cpp-source.html
    rfbKeySym k = e->nativeVirtualKey();

    // we do not handle Key_Backtab separately as the Shift-modifier
    // is already enabled
    if (e->key() == Qt::Key_Backtab) {
        k = XK_Tab;
    }

    const bool pressed = (e->type() == QEvent::KeyPress);

    // handle modifiers
    if (k == XK_Shift_L || k == XK_Control_L || k == XK_Meta_L || k == XK_Alt_L) {
        if (pressed) {
            m_mods[k] = true;
        } else if (m_mods.contains(k)) {
            m_mods.remove(k);
        } else {
            unpressModifiers();
        }
    }

    if (k) {
        vncThread.keyEvent(k, pressed);
    }
}

void VncView::unpressModifiers()
{
    const QList<unsigned int> keys = m_mods.keys();
    QList<unsigned int>::const_iterator it = keys.constBegin();
    while (it != keys.end()) {
        vncThread.keyEvent(*it, false);
        it++;
    }
    m_mods.clear();
}

void VncView::clipboardDataChanged()
{
    //kDebug(5011);

    if (m_status != Connected)
        return;

    if (m_clipboard->ownsClipboard() || m_dontSendClipboard)
        return;

    const QString text = m_clipboard->text(QClipboard::Clipboard);

    vncThread.clientCut(text);
}

VncWidget::VncWidget( QObject* parent )
{
    qDebug() << "[VNCWIDGET] Constructing";
    
    mVncView = 0;
    mLayout = new QGridLayout( this );
    setLayout( mLayout );
    
    show();
}

VncWidget::~VncWidget()
{
    qDebug() << "[VNCWIDGET] Destroying id" << mTabId;
}

void VncWidget::createVncView( const QUrl& url )
{
    if ( mVncView != 0 )
    {
        mLayout->removeWidget( mVncView );
        mVncView->startQuitting();
        delete mVncView;
        mVncView = 0;
    }
    
    mVncView = new VncView( this, url );
    
    connect( mVncView, SIGNAL( somethingWentWrong() ), 
             this, SIGNAL( somethingWentWrong() ) );
    
    mLayout->addWidget( mVncView );
    mVncView->show();
    mVncView->start();
}

void VncWidget::setTabId( int id )
{
    mTabId = id;
}

void VncWidget::getFocus( bool b )
{
    if ( mVncView != 0 )
    {
        if ( b )
        {
            mVncView->grabKeyboard();
        }
        else
        {
            mVncView->releaseKeyboard();
        }
    }
}

bool VncWidget::event( QEvent* e )
{
    if ( e->type() == QEvent::WindowDeactivate )
    {
        getFocus( false );
    }
    else if ( e->type() == QEvent::Enter )
    {
        getFocus( true );
    }
    
    QWidget::event( e );
}

void VncWidget::closeWidget()
{
    emit widgetClosed( mTabId );
}

#include "vncview.moc"

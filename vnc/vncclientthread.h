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

#ifndef VNCCLIENTTHREAD_H
#define VNCCLIENTTHREAD_H

#include <QDebug>
#define kDebug(n) qDebug()
#define kBacktrace() ""
#define i18n tr

#include "remoteview.h"

#include <QQueue>
#include <QThread>
#include <QImage>
#include <QMutex>

extern "C" {
#include <rfb/rfbclient.h>
}

class ClientEvent
{
public:
    virtual ~ClientEvent();

    virtual void fire(rfbClient*) = 0;
};

class KeyClientEvent : public ClientEvent
{
public:
    KeyClientEvent(int key, int pressed)
            : m_key(key), m_pressed(pressed) {}

    void fire(rfbClient*);

private:
    int m_key;
    int m_pressed;
};

class PointerClientEvent : public ClientEvent
{
public:
    PointerClientEvent(int x, int y, int buttonMask)
            : m_x(x), m_y(y), m_buttonMask(buttonMask) {}

    void fire(rfbClient*);

private:
    int m_x;
    int m_y;
    int m_buttonMask;
};

class ClientCutEvent : public ClientEvent
{
public:
    ClientCutEvent(const QString &text)
            : text(text) {}

    void fire(rfbClient*);

private:
    QString text;
};

class VncClientThread: public QThread
{
    Q_OBJECT

public:
    Q_ENUMS(ColorDepth)

    enum ColorDepth {
        bpp32,
        bpp16,
        bpp8
    };

    explicit VncClientThread(QObject *parent = 0);
    ~VncClientThread();
    const QImage image(int x = 0, int y = 0, int w = 0, int h = 0);
    void setImage(const QImage &img);
    void emitUpdated(int x, int y, int w, int h);
    void emitGotCut(const QString &text);
    void stop();
    void setHost(const QString &host);
    void setPort(int port);
    void setQuality(RemoteView::Quality quality);
    void setPassword(const QString &password) {
        m_password = password;
    }
    const QString password() const {
        return m_password;
    }

    RemoteView::Quality quality() const;
    ColorDepth colorDepth() const;
    uint8_t *frameBuffer;

signals:
    void imageUpdated(int x, int y, int w, int h);
    void gotCut(const QString &text);
    void passwordRequest();
    void outputErrorMessage(const QString &message);

public slots:
    void mouseEvent(int x, int y, int buttonMask);
    void keyEvent(int key, bool pressed);
    void clientCut(const QString &text);

protected:
    void run();

private:
    static void setClientColorDepth(rfbClient *cl, ColorDepth cd);
    void setColorDepth(ColorDepth colorDepth);
    //these static methods are callback functions for libvncclient
    static rfbBool newclient(rfbClient *cl);
    static void updatefb(rfbClient *cl, int x, int y, int w, int h);
    static void cuttext(rfbClient *cl, const char *text, int textlen);
    static char* passwdHandler(rfbClient *cl);
    static void outputHandler(const char *format, ...);

    QImage m_image;
    rfbClient *cl;
    QString m_host;
    QString m_password;
    int m_port;
    QMutex mutex;
    RemoteView::Quality m_quality;
    ColorDepth m_colorDepth;
    QQueue<ClientEvent* > m_eventQueue;
    //color table for 8bit indexed colors
    static QVector<QRgb> m_colorTable;

    volatile bool m_stopped;
    volatile bool m_passwordError;

private slots:
    void checkOutputErrorMessage();
};

#endif

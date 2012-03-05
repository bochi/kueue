/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
         This file has been copied and modified from arora
                 http://code.google.com/p/arora/
                  
         (C) 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>

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

#include "webviewsearch.h"

#include <qevent.h>
#include <qshortcut.h>
#include <qtimeline.h>

#include <qwebframe.h>
#include <qwebview.h>

#include <qdebug.h>

WebViewSearch::WebViewSearch(QWebView *webView, QWidget *parent)
    : SearchBar(parent)
{
    qDebug() << "[WEBVIEWSEARCH] Constructing" << webView->objectName();
    setSearchObject(webView);
#if QT_VERSION >= 0x040600 || defined(WEBKIT_TRUNK)
    ui.highlightAllButton->setVisible(true);
    connect(ui.highlightAllButton, SIGNAL(toggled(bool)),
            this, SLOT(highlightAll()));
    connect(ui.searchLineEdit, SIGNAL(textEdited(const QString &)),
            this, SLOT(highlightAll()));
#endif
}

void WebViewSearch::findNext()
{
    find(QWebPage::FindWrapsAroundDocument);
}

void WebViewSearch::findPrevious()
{
    find(QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
}

#if QT_VERSION >= 0x040600 || defined(WEBKIT_TRUNK)
void WebViewSearch::highlightAll()
{
    webView()->findText( QString(), QWebPage::HighlightAllOccurrences );

    if ( ui.highlightAllButton->isChecked() )
        find( QWebPage::HighlightAllOccurrences );
}
#endif

void WebViewSearch::find(QWebPage::FindFlags flags)
{
    QString searchString = ui.searchLineEdit->text();
    if (!searchObject() || searchString.isEmpty())
        return;
    QString infoString;
    if (!webView()->findText(searchString, flags))
        infoString = tr("Not Found");
    webView()->findText(searchString, QWebPage::HighlightAllOccurrences );
    ui.searchInfo->setText(infoString);
}

QWebView *WebViewSearch::webView() const
{
    return qobject_cast<QWebView*>(searchObject());
}

WebViewWithSearch::WebViewWithSearch( QWebView *webView, QWidget *parent )
    : QWidget(parent)
    , m_webView( webView )
{
    qDebug() << "[WEBVIEWWITHSEARCH] Constructing" << webView->objectName();
    m_webView->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_webViewSearch = new WebViewSearch( m_webView, this );
    layout->addWidget(m_webViewSearch);
    layout->addWidget(m_webView);
    setLayout(layout);
}

#include "webviewsearch.moc"

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

#ifndef WEBVIEWSEARCH_H
#define WEBVIEWSEARCH_H

#include "searchbar.h"

#include <qwebpage.h>
#include <QWebView>

QT_BEGIN_NAMESPACE
class QWebView;
QT_END_NAMESPACE

class WebViewSearch : public SearchBar
{
    Q_OBJECT

public:
    WebViewSearch(QWebView *webView, QWidget *parent = 0);

public slots:
    void findNext();
    void findPrevious();
#if QT_VERSION >= 0x040600 || defined(WEBKIT_TRUNK)
    void highlightAll();
#endif

private:
    void find(QWebPage::FindFlags flags);
    QWebView *webView() const;
};

class WebViewWithSearch : public QWidget
{
    Q_OBJECT

public:
    WebViewWithSearch(QWebView *webView, QWidget *parent = 0);
    QWebView *m_webView;
    WebViewSearch *m_webViewSearch;
};


#endif // WEBVIEWSEARCH_H


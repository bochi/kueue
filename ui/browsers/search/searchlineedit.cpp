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

#include "searchlineedit.h"

#include "clearbutton.h"
#include "searchbutton.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : LineEdit(parent)
{
    init();
}

void SearchLineEdit::init()
{
    // search button on the left
    m_searchButton = new SearchButton(this);
    addWidget(m_searchButton, LeftSide);

    // clear button on the right
    m_clearButton = new ClearButton(this);
    connect(m_clearButton, SIGNAL(clicked()),
            this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)),
            m_clearButton, SLOT(textChanged(const QString&)));
    addWidget(m_clearButton, RightSide);
    m_clearButton->hide();

    updateTextMargins();
    setInactiveText(tr("Search"));
}

ClearButton *SearchLineEdit::clearButton() const
{
    return m_clearButton;
}

SearchButton *SearchLineEdit::searchButton() const
{
    return m_searchButton;
}

#include "searchlineedit.moc"

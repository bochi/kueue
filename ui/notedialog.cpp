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

#include "notedialog.h"

#include <QDebug>

NoteDialog::NoteDialog( QObject* parent, QString sr )
{
    qDebug() << "[NOTEDIALOG] Constructing";

    mSr = sr;
    setupUi( this );
    
    connect( this, SIGNAL( accepted() ), 
             this, SLOT( noteAccepted() ) );
    
    connect( checkBoxInt, SIGNAL( toggled( bool ) ),
             this, SLOT( changeCheckBoxExt( bool ) ) );
    
    connect( checkBoxExt, SIGNAL( toggled( bool ) ),
             this, SLOT( changeCheckBoxInt( bool ) ) );
    
    titleLabel->setText( "<b>Add an engineer note to SR#" + mSr + "</b>" );
}

NoteDialog::~NoteDialog()
{
    qDebug() << "[NOTEDIALOG] Destroying";
}

void NoteDialog::changeCheckBoxInt( bool checked )
{
    checkBoxInt->setChecked( !checked );
}

void NoteDialog::changeCheckBoxExt( bool checked )
{
    checkBoxExt->setChecked( !checked );
}

void NoteDialog::noteAccepted()
{
    if ( checkBoxInt->isChecked() )
    {
        mType = NoteDialog::Internal;
    }
    else
    {
        mType = NoteDialog::Public;
    }
    
    emit addNoteAccepted();
}

void NoteDialog::closeEvent( QCloseEvent* event )
{
    emit rejected();
    QDialog::closeEvent( event );
}

#include "notedialog.moc"

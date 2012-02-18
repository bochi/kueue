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

#include <QDebug>

#include "calendardialog.h"
#include "settings.h"

CalendarDialog::CalendarDialog( QObject* parent, QString sr )
{
    qDebug() << "[CALENDARDIALOG] Constructing";
    
    mSr = sr;
     
    setupUi( this ); 
       
    mCalWidget = new QCalendarWidget( this );
    mDateTimeEdit = new QDateTimeEdit( this );
    
    mDateTimeEdit->setDate( mCalWidget->selectedDate() );
    
    calendarLayout->addWidget( mCalWidget );
    calendarLayout->addWidget( mDateTimeEdit );

    setWindowTitle( "Schedule For Close" );
    textLabel->setText( "<b>Schedule For Close for SR#" + mSr + "</b>" );
    
    connect( this, SIGNAL( accepted() ), 
             this, SLOT( calAccepted() ) );
    
    connect( mCalWidget, SIGNAL( selectionChanged() ),
             this, SLOT( calendarDateChanged() ) );
    
    connect( mDateTimeEdit, SIGNAL( dateChanged( QDate ) ),
             this, SLOT( widgetDateChanged() ) );
}

CalendarDialog::~CalendarDialog()
{
    qDebug() << "[CALENDARDIALOG] Destroying";
}

void CalendarDialog::calendarDateChanged()
{
    mDateTimeEdit->setDate( mCalWidget->selectedDate() );
}

void CalendarDialog::widgetDateChanged()
{
    mCalWidget->setSelectedDate( mDateTimeEdit->date() );
}

void CalendarDialog::calAccepted()
{
    emit datePicked( mDateTimeEdit->dateTime(), mSr );
}

#include "calendardialog.moc"

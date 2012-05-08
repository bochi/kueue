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

#ifndef UNITYWIDGETS_H
#define UNITYWIDGETS_H
 
#include <QDateTime>
#include <QCalendarWidget>
#include <QDateTimeEdit>

#include "ui_calendardialog.h"
#include "ui_closedialog.h"
#include "ui_notedialog.h"
#include "ui_ssdialog.h"
#include "ui_passwordprompt.h"

class CalendarDialog : public QDialog, private Ui::CalendarDialog
{
    Q_OBJECT

    public:
        CalendarDialog( QObject* parent = 0L, QString sr = 0 );
         ~CalendarDialog();
         
         QDateTime dateTime() { return mDateTimeEdit->dateTime(); }
         QString sr() { return mSr; }
            
    private:
        QString mSr;
        QCalendarWidget* mCalWidget;
        QDateTimeEdit* mDateTimeEdit;
    
    private slots:
        void calendarDateChanged();
        void widgetDateChanged();

    protected:
        void closeEvent( QCloseEvent* event );
};

class SsDialog : public QDialog, private Ui::SsDialog
{
    Q_OBJECT

    public:
        SsDialog( QObject* parent = 0L, QString sr = 0 );
         ~SsDialog();
         
         QString sr() { return mSr; }
         QString ssText() { return reasonLineEdit->text(); }
        
    private: 
        QString mSr;
        
    protected:
        void closeEvent( QCloseEvent* event );  
};
 
class CloseDialog : public QDialog, private Ui::CloseDialog
{
    Q_OBJECT

    public:
        CloseDialog( QObject* parent = 0L, const QString& sr = "NONE" );
         ~CloseDialog();
  
         QString sr() { return mSr; }
         QString closeReason() { return closeCombo->currentText(); }
         
    private:
        QString mSr;
        
    protected:
        void closeEvent( QCloseEvent *event );
};

class NoteDialog : public QDialog, private Ui::NoteDialog
{
    Q_OBJECT

    public:
        NoteDialog( QObject* parent = 0L, QString = "NOSR", bool = false );
         ~NoteDialog();
        
        enum type
        {
            Internal = 1,
            Public = 2
        };
 
        QString sr() { return mSr; }
        QString briefDesc() { return briefDescription->text(); }
        QString comment() { return commentBox->toPlainText(); }
        QString noteType() { return noteTypeCombo->currentText(); }
        int type() { return mType; }
        
    private:
        QString mSr;
        int mType;
        
    private slots:
        void noteAccepted();
        void changeCheckBoxInt( bool );
        void changeCheckBoxExt( bool );
        
    protected:
        void closeEvent( QCloseEvent *event );
        
    signals:
        void addNoteAccepted();
};

class PasswordPrompt : public QDialog, private Ui::PasswordPrompt
{
    Q_OBJECT

    public:
        PasswordPrompt( QObject* parent = 0L );
         ~PasswordPrompt();
        
    private slots:
        void passwordEntered();
        
    signals:
        void passwordReceived();
        
};
  
#endif

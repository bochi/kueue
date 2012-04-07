/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
         This file has been copied and modified from arora
                 http://code.google.com/p/arora/
                  
       (C) 2008-2009 Benjamin C. Meyer <ben@meyerhome.net
          (C) 2009 Jakub Wieczorek <faw217@gmail.com> 

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

#include "edittableview.h"

#include <qevent.h>

EditTableView::EditTableView(QWidget *parent)
    : QTableView(parent)
{
}

void EditTableView::keyPressEvent(QKeyEvent *event)
{
    if (model() && event->key() == Qt::Key_Delete) {
        removeSelected();
        event->setAccepted(true);
    } else {
        QAbstractItemView::keyPressEvent(event);
    }
}

void EditTableView::removeSelected()
{
    if (!model() || !selectionModel() || !selectionModel()->hasSelection())
        return;

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    int newSelectedRow = selectedRows.at(0).row();
    for (int i = selectedRows.count() - 1; i >= 0; --i) {
        QModelIndex idx = selectedRows.at(i);
        model()->removeRow(idx.row(), rootIndex());
    }

    // select the item at the same position
    QModelIndex newSelectedIndex = model()->index(newSelectedRow, 0, rootIndex());
    // if that was the last item
    if (!newSelectedIndex.isValid())
        newSelectedIndex = model()->index(newSelectedRow - 1, 0, rootIndex());

    selectionModel()->select(newSelectedIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    setCurrentIndex(newSelectedIndex);
}

void EditTableView::removeAll()
{
    if (!model())
        return;

    model()->removeRows(0, model()->rowCount(rootIndex()), rootIndex());
}

#include "edittableview.moc"

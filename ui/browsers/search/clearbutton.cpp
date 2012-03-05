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

#include "clearbutton.h"

#include <qpainter.h>

ClearButton::ClearButton(QWidget *parent)
    : QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
    setToolTip(tr("Clear"));
    setMinimumSize(22, 22);
    setVisible(false);

#if QT_VERSION >= 0x040600
    // First check for a style icon, current KDE provides one
    if (m_styleImage.isNull()) {
        QLatin1String iconName = (layoutDirection() == Qt::RightToLeft)
            ? QLatin1String("edit-clear-locationbar-ltr")
            : QLatin1String("edit-clear-locationbar-rtl");
        QIcon icon = QIcon::fromTheme(iconName);
        if (!icon.isNull())
            m_styleImage = icon.pixmap(16, 16).toImage();
    }
#endif
}

void ClearButton::textChanged(const QString &text)
{
    setVisible(!text.isEmpty());
}

void ClearButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (!m_styleImage.isNull()) {
        int x = (width() - m_styleImage.width()) / 2 - 1;
        int y = (height() - m_styleImage.height()) / 2 - 1;
        painter.drawImage(x, y, m_styleImage);
        return;
    }

    // Fall back to boring circle X
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPalette p = palette();
    QColor circleColor = isDown() ? p.color(QPalette::Dark) : p.color(QPalette::Mid);
    QColor xColor = p.color(QPalette::Window);

    // draw circle
    painter.setBrush(circleColor);
    painter.setPen(circleColor);
    int padding = width() / 5;
    int circleRadius = width() - (padding * 2);
    painter.drawEllipse(padding, padding, circleRadius, circleRadius);

    // draw X
    painter.setPen(xColor);
    padding = padding * 2;
    painter.drawLine(padding, padding,            width() - padding, width() - padding);
    painter.drawLine(padding, height() - padding, width() - padding, padding);
}

#include "clearbutton.moc"

/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>
             
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

#ifndef SQUEEZELABEL_H
#define SQUEEZELABEL_H

#include <qlabel.h>

/*
    A label that will squeeze the set text to fit within the size of the
    widget.  The text will be elided in the middle.
 */
class SqueezeLabel : public QLabel
{
    Q_OBJECT

public:
    SqueezeLabel(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QString m_SqueezedTextCache;
};

#endif // SQUEEZELABEL_H


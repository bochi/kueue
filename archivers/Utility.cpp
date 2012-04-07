/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
            Thanks to QComicBook for the inspiration :-)
     Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
    
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

#include "Utility.h"
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo>
#include <stdlib.h>
#include <utime.h>
#include "config.h"

QString Utility::which(const QString &command)
{
	const QString paths = QString(getenv("PATH"));
#ifdef IS_WIN32
	QStringList plist = paths.split(";");
	plist.prepend(QCoreApplication::applicationDirPath());
#else
	QStringList plist = paths.split(":");
#endif
	for (QStringList::const_iterator it = plist.begin(); it != plist.end(); it++)
	{
#ifdef IS_WIN32
		QFileInfo finfo(*it + "/" + command + ".exe");
#else
		QFileInfo finfo(*it + "/" + command);
#endif
		if (finfo.isExecutable())
			return finfo.absoluteFilePath();
	}
	return QString::null;
}

void Utility::touch(const QString &fname)
{
    utime(fname.toLocal8Bit(), NULL);
}

QString Utility::shortenPath(const QString &path, const QString &filler, int maxlen)
{
    if (path.length() > maxlen)
    {
        QString shortPath("/" + filler );
        QFileInfo finf(path);
        maxlen -= finf.fileName().length() + shortPath.length();
        const QString p(finf.absolutePath());
        if (p.length() > shortPath.length())
        {
            if (maxlen>0)
            {
                int i = p.length() - maxlen;
                for (; i<p.length() && p[i]!='/'; i++);
                shortPath.append(p.right(p.length() - i ));
            }
            shortPath.append("/");            
            shortPath.append(finf.fileName());
            return shortPath;
        }
    }
    return path;
}

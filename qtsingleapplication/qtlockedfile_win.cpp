/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
           This file is part of a Qt Solutions component.
      (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
    
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

#include "qtlockedfile.h"
#include <qt_windows.h>
#include <QtCore/QFileInfo>

#define MUTEX_PREFIX "QtLockedFile mutex "
// Maximum number of concurrent read locks. Must not be greater than MAXIMUM_WAIT_OBJECTS
#define MAX_READERS MAXIMUM_WAIT_OBJECTS

Qt::HANDLE QtLockedFile::getMutexHandle(int idx, bool doCreate)
{
    if (mutexname.isEmpty()) {
        QFileInfo fi(*this);
        mutexname = QString::fromLatin1(MUTEX_PREFIX)
                    + fi.absoluteFilePath().toLower();
    }
    QString mname(mutexname);
    if (idx >= 0)
        mname += QString::number(idx);

    Qt::HANDLE mutex;
    if (doCreate) {
        QT_WA( { mutex = CreateMutexW(NULL, FALSE, (WCHAR*)mname.utf16()); },
               { mutex = CreateMutexA(NULL, FALSE, mname.toLocal8Bit().constData()); } );
        if (!mutex) {
            qErrnoWarning("QtLockedFile::lock(): CreateMutex failed");
            return 0;
        }
    }
    else {
        QT_WA( { mutex = OpenMutexW(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE, (WCHAR*)mname.utf16()); },
               { mutex = OpenMutexA(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE, mname.toLocal8Bit().constData()); } );
        if (!mutex) {
            if (GetLastError() != ERROR_FILE_NOT_FOUND)
                qErrnoWarning("QtLockedFile::lock(): OpenMutex failed");
            return 0;
        }
    }
    return mutex;
}

bool QtLockedFile::waitMutex(Qt::HANDLE mutex, bool doBlock)
{
    Q_ASSERT(mutex);
    DWORD res = WaitForSingleObject(mutex, doBlock ? INFINITE : 0);
    switch (res) {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
        return true;
        break;
    case WAIT_TIMEOUT:
        break;
    default:
        qErrnoWarning("QtLockedFile::lock(): WaitForSingleObject failed");
    }
    return false;
}



bool QtLockedFile::lock(LockMode mode, bool block)
{
    if (!isOpen()) {
        qWarning("QtLockedFile::lock(): file is not opened");
        return false;
    }

    if (mode == NoLock)
        return unlock();

    if (mode == m_lock_mode)
        return true;

    if (m_lock_mode != NoLock)
        unlock();

    if (!wmutex && !(wmutex = getMutexHandle(-1, true)))
        return false;

    if (!waitMutex(wmutex, block))
        return false;

    if (mode == ReadLock) {
        int idx = 0;
        for (; idx < MAX_READERS; idx++) {
            rmutex = getMutexHandle(idx, false);
            if (!rmutex || waitMutex(rmutex, false))
                break;
            CloseHandle(rmutex);
        }
        bool ok = true;
        if (idx >= MAX_READERS) {
            qWarning("QtLockedFile::lock(): too many readers");
            rmutex = 0;
            ok = false;
        }
        else if (!rmutex) {
            rmutex = getMutexHandle(idx, true);
            if (!rmutex || !waitMutex(rmutex, false))
                ok = false;
        }
        if (!ok && rmutex) {
            CloseHandle(rmutex);
            rmutex = 0;
        }
        ReleaseMutex(wmutex);
        if (!ok)
            return false;
    }
    else {
        Q_ASSERT(rmutexes.isEmpty());
        for (int i = 0; i < MAX_READERS; i++) {
            Qt::HANDLE mutex = getMutexHandle(i, false);
            if (mutex)
                rmutexes.append(mutex);
        }
        if (rmutexes.size()) {
            DWORD res = WaitForMultipleObjects(rmutexes.size(), rmutexes.constData(),
                                               TRUE, block ? INFINITE : 0);
            if (res != WAIT_OBJECT_0 && res != WAIT_ABANDONED) {
                if (res != WAIT_TIMEOUT)
                    qErrnoWarning("QtLockedFile::lock(): WaitForMultipleObjects failed");
                m_lock_mode = WriteLock;  // trick unlock() to clean up - semiyucky
                unlock();
                return false;
            }
        }
    }

    m_lock_mode = mode;
    return true;
}

bool QtLockedFile::unlock()
{
    if (!isOpen()) {
        qWarning("QtLockedFile::unlock(): file is not opened");
        return false;
    }

    if (!isLocked())
        return true;

    if (m_lock_mode == ReadLock) {
        ReleaseMutex(rmutex);
        CloseHandle(rmutex);
        rmutex = 0;
    }
    else {
        foreach(Qt::HANDLE mutex, rmutexes) {
            ReleaseMutex(mutex);
            CloseHandle(mutex);
        }
        rmutexes.clear();
        ReleaseMutex(wmutex);
    }

    m_lock_mode = QtLockedFile::NoLock;
    return true;
}

QtLockedFile::~QtLockedFile()
{
    if (isOpen())
        unlock();
    if (wmutex)
        CloseHandle(wmutex);
}

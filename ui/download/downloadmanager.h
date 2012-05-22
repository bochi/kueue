/*
                kueue - keep track of your SR queue
          (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
             
         This file has been copied and modified from arora
                 http://code.google.com/p/arora/
                  
       (C) 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
          (C) 2008 Jason A. Donenfeld <Jason@zx2c4.com>
 
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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "ui_downloads.h"
#include "ui_downloaditem.h"

#include <qnetworkreply.h>
#include "unity/unitynetwork.h"

#include <qfile.h>
#include <qdatetime.h>

class DownloadItem : public QWidget, public Ui_DownloadItem
{
    Q_OBJECT

signals:
    void statusChanged();
    void progress(qint64 bytesReceived = 0, qint64 bytesTotal = 0);
    void downloadFinished();

public:
    DownloadItem( QNetworkReply *reply = 0, bool requestFileName = false, QString dir = "bla", QWidget *parent = 0);
    bool downloading() const;
    bool downloadedSuccessfully() const;

    qint64 bytesTotal() const;
    qint64 bytesReceived() const;
    double remainingTime() const;
    double currentSpeed() const;

    QUrl mUrl;

    QFile mOutput;
    QNetworkReply *mReply;
    

private slots:
    void stop();
    void tryAgain();
    void open();

    void downloadReadyRead();
    void error(QNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void metaDataChanged();
    void generateNsaReport();
    void nsaFinished();
    void finished();

private:
    void getFileName();
    void init();
    void updateInfoLabel();

    QString saveFileName(const QString &directory) const;

    bool mRequestFilename;
    qint64 mBytesReceived;
    QTime mDownloadTime;
    bool mStartedSaving;
    bool mFinishedDownloading;
    bool mGettingFilename;
    bool mCanceledFileSelect;
    QTime mLastProgressTime;
    QString mDownloadDir;
    
    friend class DownloadManager;
};

class AutoSaver;
class DownloadModel;
QT_BEGIN_NAMESPACE
class QFileIconProvider;
class QMimeData;
QT_END_NAMESPACE

class DownloadManager : public QWidget, public Ui_DownloadDialog
{
    Q_OBJECT
    Q_PROPERTY(RemovePolicy removePolicy READ removePolicy WRITE setRemovePolicy)
    Q_ENUMS(RemovePolicy)

public:
    enum RemovePolicy {
        Never,
        Exit,
        SuccessFullDownload
    };

    DownloadManager( QWidget *parent = 0 );
    ~DownloadManager();
    int activeDownloads() const;
    bool allowQuit();

    RemovePolicy removePolicy() const;
    void setRemovePolicy(RemovePolicy policy);

    static QString timeString(double timeRemaining);
    static QString dataString(qint64 size);

    void setDownloadDirectory(const QString &directory);
    QString downloadDirectory();

public slots:
    void download(const QNetworkRequest &request, QNetworkAccessManager* nam, QString dir, bool requestFileName = false);
    inline void download(const QUrl &url, QNetworkAccessManager* nam, QString dir, bool requestFileName = false)
        { download(QNetworkRequest(url), nam, dir, requestFileName); }
    void handleUnsupportedContent(QNetworkReply *reply, QString dir, bool requestFileName = false);
    void cleanup();

private slots:
    void save() const;
    void updateRow(DownloadItem *item);
    void updateRow();
    void finished();

private:
    void addItem(DownloadItem *item);
    void updateItemCount();
    void load();
    bool externalDownload(const QUrl &url);
    void updateActiveItemCount();

    AutoSaver *mAutoSaver;
    DownloadModel *mModel;
    QFileIconProvider *mIconProvider;
    QList<DownloadItem*> mDownloads;
    RemovePolicy mRemovePolicy;
    QString mDownloadDir;

    friend class DownloadModel;

signals:
    void downloadFinished();
};

class DownloadModel : public QAbstractListModel
{
    friend class DownloadManager;
    Q_OBJECT

public:
    DownloadModel(DownloadManager *downloadManager, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    DownloadManager *mDownloadManager;

};

#endif // DOWNLOADMANAGER_H


/*
                kueue - keep track of your SR queue
             (C) 2011 Stefan Bogner <sbogner@suse.com>
             
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

#include "downloadmanager.h"
#include "autosaver.h"
#include "unity/archiveextract.h"
#include "settings.h"
#include "kueue.h"
#include "kueuethreads.h"
#include "nsa/nsa.h"

#include <math.h>

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QHeaderView>
#include <QMessageBox>
#include <QMetaObject>
#include <QMimeData>
#include <QProcess>
#include <QSettings>
#include <QWebSettings>
#include <QDebug>

//#define DOWNLOADMANAGER_DEBUG

/*!
    DownloadItem is a widget that is displayed in the download manager list.
    It moves the data from the QNetworkReply into the QFile as well
    as update the information/progressbar and report errors.
 */

DownloadItem::DownloadItem( QNetworkReply *reply, bool requestFileName, QString dir, QWidget *parent )
    : QWidget( parent )
    , mReply( reply )
    , mRequestFilename( requestFileName )
    , mBytesReceived( 0 )
    , mStartedSaving( false )
    , mFinishedDownloading( false )
    , mGettingFilename( false )
    , mCanceledFileSelect( false )
    , mDownloadDir( dir + "/" )
{
    setupUi( this );
    
    QPalette p = downloadInfoLabel->palette();
    p.setColor(QPalette::Text, Qt::darkGray);
    downloadInfoLabel->setPalette(p);
    
    nsaButton->setVisible( false );
    
    progressBar->setMaximum(0);
    
    connect( stopButton, SIGNAL( clicked() ), 
             this, SLOT( stop() ) );
    
    connect( openButton, SIGNAL( clicked() ), 
             this, SLOT( open() ) );

    connect( nsaButton, SIGNAL( clicked() ),
             this, SLOT( generateNsaReport() ) );

    init();
}

void DownloadItem::init()
{
    if ( !mReply )
    {
        return;
    }

    mStartedSaving = false;
    
    mFinishedDownloading = false;

    openButton->setEnabled(false);

    // attach to the mReply
    mUrl = mReply->url();
    mReply->setParent( this );
    
    connect( mReply, SIGNAL( readyRead() ), 
             this, SLOT( downloadReadyRead() ) );
    
    connect( mReply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( error( QNetworkReply::NetworkError ) ) );
    
    connect( mReply, SIGNAL( downloadProgress( qint64, qint64 ) ),
             this, SLOT( downloadProgress( qint64, qint64 ) ) );
    
    connect( mReply, SIGNAL( metaDataChanged() ),
             this, SLOT( metaDataChanged() ) );
    
    connect( mReply, SIGNAL( finished() ),
             this, SLOT( finished() ) );

    // reset info
    downloadInfoLabel->clear();
    progressBar->setValue(0);
    getFileName();

    // start timer for the download estimation
    mDownloadTime.start();

    if (mReply->error() != QNetworkReply::NoError) {
        error(mReply->error());
        finished();
    }
}

void DownloadItem::getFileName()
{
    if ( mGettingFilename )
    {
        return;
    }

    QString downloadDirectory = mDownloadDir;

    QString defaultFileName = saveFileName( downloadDirectory );
    QString fileName = defaultFileName;
    
    if ( mRequestFilename ) 
    {
        mGettingFilename = true;
        fileName = QFileDialog::getSaveFileName( this, "Save File", defaultFileName );
        mGettingFilename = false;

        if ( fileName.isEmpty() ) 
        {
            progressBar->setVisible( false );
            stop();
            fileNameLabel->setText( QString("Download canceled: %1").arg(QFileInfo(defaultFileName).fileName() ) );
            mCanceledFileSelect = true;
            return;
        }
        
        QFileInfo fileInfo = QFileInfo( fileName );
        fileNameLabel->setText( fileInfo.fileName() );
    }

    mOutput.setFileName( fileName );

    // Check file path for saving.
    QDir saveDirPath = QFileInfo( mOutput.fileName() ).dir();
    
    if ( !saveDirPath.exists() ) 
    {
        if ( !saveDirPath.mkpath( saveDirPath.absolutePath() ) ) 
        {
            progressBar->setVisible( false );
            stop();
            downloadInfoLabel->setText(QString( "Download directory (%1) couldn't be created." ).arg(saveDirPath.absolutePath() ) );
            return;
        }
    }

    fileNameLabel->setText( QFileInfo( mOutput.fileName() ).fileName() );
    
    if ( mRequestFilename )
    {
        downloadReadyRead();
    }
}

QString DownloadItem::saveFileName( const QString &directory ) const
{
    // Move this function into QNetworkReply to also get file name sent from the server
    QString path;
    
    if ( mReply->hasRawHeader("Content-Disposition") ) 
    {
        QString value = QLatin1String( mReply->rawHeader( "Content-Disposition" ) );
        int pos = value.indexOf( QLatin1String("filename=") );
        
        if ( pos != -1 ) 
        {
            QString name = value.mid( pos + 9 );
            
            if ( name.startsWith( QLatin1Char( '"' ) ) && name.endsWith( QLatin1Char( '"' ) ) )
            {
                name = name.mid( 1, name.size() - 2 );
            }
            
            path = name;
        }
    }
    
    if ( path.isEmpty() )
    {
        path = mUrl.path();
    }

    QFileInfo info( path );
    
    QString baseName = info.completeBaseName();
    QString endName = info.suffix();

    if ( baseName.isEmpty() ) 
    {
        baseName = QLatin1String( "unnamed_download" );
        qDebug() << "DownloadItem::" << __FUNCTION__ << "downloading unknown file:" << mUrl;
    }

    if ( !endName.isEmpty() )
    {
        endName = QLatin1Char('.') + endName;
    }

    QString name = directory + baseName + endName;
    
    if ( !mRequestFilename && QFile::exists( name ) ) 
    {
        int i = 1;
        
        do 
        {
            name = directory + baseName + QLatin1Char( '-' ) + QString::number( i++ ) + endName;
        } 
        while ( QFile::exists( name ) );
    }
    
    return name;
}

void DownloadItem::stop()
{
    setUpdatesEnabled( false );
    stopButton->setEnabled( false );
    stopButton->hide();
    setUpdatesEnabled( true );
    mReply->abort();
    emit downloadFinished();
}

void DownloadItem::open()
{
    QFileInfo info( mOutput );
    QUrl url = QUrl::fromLocalFile( info.absoluteFilePath() );
    QDesktopServices::openUrl( url );
}

void DownloadItem::tryAgain()
{
    /*if ( !tryAgainButton->isEnabled() )
    {
        return;
    }

    tryAgainButton->setEnabled( false );
    tryAgainButton->setVisible( false );
    stopButton->setEnabled( true );
    stopButton->setVisible( true );
    progressBar->setVisible( true );

    QNetworkReply *r;// = BrowserApplication::networkAccessManager()->get(QNetworkRequest(mUrl));
    if (mReply)
        mReply->deleteLater();
    if (mOutput.exists())
        mOutput.remove();
    mReply = r;
    init();
    emit statusChanged();*/
}

void DownloadItem::downloadReadyRead()
{
    if ( mRequestFilename && mOutput.fileName().isEmpty() )
    {
        return;
    }
    
    if ( !mOutput.isOpen() ) 
    {
        // in case someone else has already put a file there
        if ( !mRequestFilename )
        {
            getFileName();
        }
        
        if ( !mOutput.open( QIODevice::WriteOnly ) ) 
        {
            downloadInfoLabel->setText( QString( "Error opening output file: %1" ).arg( mOutput.errorString() ) );
            stop();
            emit statusChanged();
            return;
        }
        
        emit statusChanged();
    }
    
    if ( -1 == mOutput.write( mReply->readAll() ) ) 
    {
        downloadInfoLabel->setText( QString( "Error saving: %1" ).arg(mOutput.errorString() ) );
        stopButton->click();
    }
    else 
    {
        mStartedSaving = true;
        
        if ( mFinishedDownloading )
        {
            finished();
        }
    }
}

void DownloadItem::error( QNetworkReply::NetworkError )
{
    qDebug() << "DownloadItem::" << __FUNCTION__ << mReply->errorString() << mUrl;

    downloadInfoLabel->setText( QString("Network Error: %1").arg( mReply->errorString() ) );
    emit downloadFinished();
}

void DownloadItem::metaDataChanged()
{
    QVariant locationHeader = mReply->header( QNetworkRequest::LocationHeader );
    
    if ( locationHeader.isValid() ) 
    {
        mUrl = locationHeader.toUrl();
        mReply->deleteLater();
        init();
        return;
    }

    qDebug() << "DownloadItem::" << __FUNCTION__ << "not handled.";
}

void DownloadItem::downloadProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    QTime now = QTime::currentTime();
    
    if ( mLastProgressTime.msecsTo( now ) < 200 )
    {
        return;
    }

    mLastProgressTime = now;

    mBytesReceived = bytesReceived;
    qint64 currentValue = 0;
    qint64 totalValue = 0;
    
    if ( bytesTotal > 0 ) 
    {
        currentValue = bytesReceived * 100 / bytesTotal;
        totalValue = 100;
    }
    
    progressBar->setValue( currentValue );
    progressBar->setMaximum( totalValue );

    emit progress( currentValue, totalValue );
    updateInfoLabel();
    
    if ( mBytesReceived == bytesTotal )
    {
        downloadReadyRead();
    }
}

qint64 DownloadItem::bytesTotal() const
{
    return mReply->header( QNetworkRequest::ContentLengthHeader ).toULongLong();
}

qint64 DownloadItem::bytesReceived() const
{
    return mBytesReceived;
}

double DownloadItem::remainingTime() const
{
    if ( !downloading() )
    {
        return -1.0;
    }

    double timeRemaining = ( ( double )( bytesTotal() - bytesReceived() ) ) / currentSpeed();

    // When downloading the eta should never be 0
    if ( timeRemaining == 0 )
    {
        timeRemaining = 1;
    }

    return timeRemaining;
}

double DownloadItem::currentSpeed() const
{
    if ( !downloading() )
    {
        return -1.0;
    }

    return mBytesReceived * 1000.0 / mDownloadTime.elapsed();
}

void DownloadItem::updateInfoLabel()
{
    if ( mReply->error() != QNetworkReply::NoError )
    {
        return;
    }

    qint64 bytesTotal = mReply->header( QNetworkRequest::ContentLengthHeader ).toULongLong();
    bool running = !downloadedSuccessfully();

    // update info label
    double speed = currentSpeed();
    double timeRemaining = remainingTime();

    QString info;
    
    if ( running ) 
    {
        QString remaining;

        if ( bytesTotal != 0 ) 
        {
            remaining = DownloadManager::timeString( timeRemaining );
        }

        info = QString(tr("%1 of %2 (%3/sec) - %4"))
            .arg(DownloadManager::dataString(mBytesReceived))
            .arg(bytesTotal == 0 ? tr("?") : DownloadManager::dataString(bytesTotal))
            .arg(DownloadManager::dataString((int)speed))
            .arg(remaining);
    } 
    else 
    {
        if ( mBytesReceived == bytesTotal )
        {
            info = DownloadManager::dataString(mOutput.size());
        }
        else
        {
            info = tr("%1 of %2 - Download Complete")
                .arg(DownloadManager::dataString(mBytesReceived))
                .arg(DownloadManager::dataString(bytesTotal));
        }
    }
    
    downloadInfoLabel->setText(info);
    
}

bool DownloadItem::downloading() const
{
    return ( progressBar->isVisible() );
}

bool DownloadItem::downloadedSuccessfully() const
{
    return ( stopButton->isHidden() );
}

void DownloadItem::finished()
{
    mFinishedDownloading = true;
    
    if ( !mStartedSaving ) 
    {
        return;
    }
    
    progressBar->hide();
    stopButton->setEnabled(false);
    stopButton->hide();
    openButton->setEnabled(true);
    
    if ( QFileInfo( mOutput.fileName() ).suffix() == "tbz" )
    {
        nsaButton->setVisible( true );
    }

    mOutput.close();
    
    if ( ( !mOutput.fileName().isEmpty() ) && 
         ( Settings::autoExtract() ) &&
         ( ( QFileInfo( mOutput.fileName() ).suffix() == "gz" ) ||
         ( QFileInfo( mOutput.fileName() ).suffix() == "bz2" ) ||
         ( QFileInfo( mOutput.fileName() ).suffix() == "tar" ) ||
         ( QFileInfo( mOutput.fileName() ).suffix() == "zip" ) ||
         ( QFileInfo( mOutput.fileName() ).suffix() == "tbz" ) ) )
    {
        ArchiveExtract* x = new ArchiveExtract( mOutput.fileName(), QFileInfo( mOutput.fileName() ).dir().absolutePath() );
        KueueThreads::enqueue( x );
    }
    
    Kueue::notify( "kueue-general", "Download finished", "<b>" + QFileInfo( mOutput.fileName() ).fileName() +
                   "</b><br>", "downloaded successfully." );
    
    updateInfoLabel();
    emit statusChanged();
    emit downloadFinished();
}

void DownloadItem::generateNsaReport()
{
    NSA* n = new NSA( mOutput.fileName() );
    
    connect( n, SIGNAL( nsaReportFinished() ),
             this, SLOT( nsaFinished() ) );
    
    nsaButton->setEnabled( false );
}

void DownloadItem::nsaFinished()
{
    nsaButton->setEnabled( true );
}

/*!
    DownloadManager is a Dialog that contains a list of DownloadItems

    It is a basic download manager.  It only downloads the file, doesn't do BitTorrent,
    extract zipped files or anything fancy.
  */
DownloadManager::DownloadManager(QWidget *parent)
    : QDialog(parent)
    , mAutoSaver(new AutoSaver(this))
    , mModel(new DownloadModel(this))
    , mIconProvider(0)
    , mRemovePolicy(Never)
{
    setupUi(this);

    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QString defaultLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    setDownloadDirectory(settings.value(QLatin1String("downloadDirectory"), defaultLocation).toString());

    downloadsView->setShowGrid(false);
    downloadsView->verticalHeader()->hide();
    downloadsView->horizontalHeader()->hide();
    downloadsView->setAlternatingRowColors(true);
    downloadsView->horizontalHeader()->setStretchLastSection(true);
    downloadsView->setModel(mModel);
    
    cleanupButton->setEnabled(false);
    
    connect(cleanupButton, SIGNAL(clicked()), this, SLOT(cleanup()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    load();
}

DownloadManager::~DownloadManager()
{
    mAutoSaver->changeOccurred();
    mAutoSaver->saveIfNeccessary();
    if (mIconProvider)
        delete mIconProvider;
}

int DownloadManager::activeDownloads() const
{
    int count = 0;
    for (int i = 0; i < mDownloads.count(); ++i) {
        if (mDownloads.at(i)->stopButton->isEnabled())
            ++count;
    }
    return count;
}

bool DownloadManager::allowQuit()
{
    if (activeDownloads() >= 1) {
        int choice = QMessageBox::warning(this, QString(),
                                        tr("There are %1 downloads in progress\n"
                                           "Do you want to quit anyway?").arg(activeDownloads()),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
        if (choice == QMessageBox::No) {
            show();
            return false;
        }
    }
    return true;
}

bool DownloadManager::externalDownload(const QUrl &url)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    if (!settings.value(QLatin1String("external"), false).toBool())
        return false;

    QString program = settings.value(QLatin1String("externalPath")).toString();
    if (program.isEmpty())
        return false;

    // Split program at every space not inside double quotes
    QRegExp regex(QLatin1String("\"([^\"]+)\"|([^ ]+)"));
    QStringList args;
    for (int pos = 0; (pos = regex.indexIn(program, pos)) != -1; pos += regex.matchedLength())
        args << regex.cap(1) + regex.cap(2);
    if (args.isEmpty())
        return false;

    return QProcess::startDetached(args.takeFirst(), args << QString::fromUtf8(url.toEncoded()));
}

void DownloadManager::download(const QNetworkRequest &request, QNetworkAccessManager* nam,  QString dir, bool requestFileName)
{
    /*if (request.url().isEmpty()){
        qDebug() << "dl nourl";
        //return;
        
    }
        
    if (externalDownload(request.url())){
        qDebug() << "dl ex";
        //return;
        
    } */
    
    handleUnsupportedContent(nam->get(request), dir, requestFileName);
}

void DownloadManager::handleUnsupportedContent(QNetworkReply *reply, QString dir, bool requestFileName)
{
    if (!reply || reply->url().isEmpty())
        return;
    if (externalDownload(reply->url()))
        return;

    QVariant header = reply->header(QNetworkRequest::ContentLengthHeader);
    bool ok;
    int size = header.toInt(&ok);
    if (ok && size == 0)
        return;

    qDebug() << "DownloadManager::" << __FUNCTION__ << reply->url() << "requestFileName" << requestFileName;

    DownloadItem *item = new DownloadItem( reply, requestFileName, dir, this );
    addItem( item );

    if (item->mCanceledFileSelect)
        return;

    /*if (!isVisible())
        show();

    activateWindow();
    raise();*/
}

void DownloadManager::addItem(DownloadItem *item)
{
    connect(item, SIGNAL(statusChanged()), this, SLOT(updateRow()));
    connect(item, SIGNAL(downloadFinished()), this, SLOT(finished()));
    int row = mDownloads.count();
    mModel->beginInsertRows(QModelIndex(), 0, 0 );
    mDownloads.prepend(item);
    mModel->endInsertRows();
    updateItemCount();
    downloadsView->setIndexWidget(mModel->index( 0, 0 ), item);
    QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(row, item->sizeHint().height());
    updateRow( item ); //incase download finishes before the constructor returns
    updateActiveItemCount();
}

void DownloadManager::updateActiveItemCount()
{
    int acCount = activeDownloads();
    if (acCount > 0) {
        setWindowTitle(QApplication::translate("DownloadDialog", "Downloading %1", 0, QApplication::UnicodeUTF8).arg(acCount));
    } else {
        setWindowTitle(QApplication::translate("DownloadDialog", "Downloads", 0, QApplication::UnicodeUTF8));
    }
}

void DownloadManager::finished()
{
    updateActiveItemCount();
    if (isVisible()) {
        QApplication::alert(this);
    }                                    
}


void DownloadManager::updateRow()
{
    if (DownloadItem *item = qobject_cast<DownloadItem*>(sender()))
        updateRow(item);
}

void DownloadManager::updateRow(DownloadItem *item)
{
    
    int row = 0;//mDownloads.indexOf(item);
    if (-1 == row)
        return;
    if (!mIconProvider)
        mIconProvider = new QFileIconProvider();
    QIcon icon = mIconProvider->icon(item->mOutput.fileName());
    if (icon.isNull())
        icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));

    int oldHeight = downloadsView->rowHeight(row);
    downloadsView->setRowHeight(row, qMax(oldHeight, item->minimumSizeHint().height()));

    bool remove = false;
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (!item->downloading()
        && globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        remove = true;

    if (item->downloadedSuccessfully()
        && removePolicy() == DownloadManager::SuccessFullDownload) {
        remove = true;
    }
    if (remove)
        mModel->removeRow(row);

    cleanupButton->setEnabled( mDownloads.count() - activeDownloads() > 0);
}

DownloadManager::RemovePolicy DownloadManager::removePolicy() const
{
    return mRemovePolicy;
}

void DownloadManager::setRemovePolicy(RemovePolicy policy)
{
    if (policy == mRemovePolicy)
        return;
    mRemovePolicy = policy;
    mAutoSaver->changeOccurred();
}

void DownloadManager::save() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    settings.setValue(QLatin1String("removeDownloadsPolicy"), QLatin1String(removePolicyEnum.valueToKey(mRemovePolicy)));
    settings.setValue(QLatin1String("size"), size());
    if (mRemovePolicy == Exit)
        return;

    for (int i = 0; i < mDownloads.count(); ++i) {
        QString key = QString(QLatin1String("download_%1_")).arg(i);
        settings.setValue(key + QLatin1String("url"), mDownloads[i]->mUrl);
        settings.setValue(key + QLatin1String("location"), QFileInfo(mDownloads[i]->mOutput).filePath());
        settings.setValue(key + QLatin1String("done"), mDownloads[i]->downloadedSuccessfully());
    }
    int i = mDownloads.count();
    QString key = QString(QLatin1String("download_%1_")).arg(i);
    while (settings.contains(key + QLatin1String("url"))) {
        settings.remove(key + QLatin1String("url"));
        settings.remove(key + QLatin1String("location"));
        settings.remove(key + QLatin1String("done"));
        key = QString(QLatin1String("download_%1_")).arg(++i);
    }
}

void DownloadManager::load()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("downloadmanager"));
    QSize size = settings.value(QLatin1String("size")).toSize();
    if (size.isValid())
        resize(size);
    QByteArray value = settings.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never")).toByteArray();
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    mRemovePolicy = removePolicyEnum.keyToValue(value) == -1 ?
                        Never :
                        static_cast<RemovePolicy>(removePolicyEnum.keyToValue(value));

    int i = 0;
    QString key = QString(QLatin1String("download_%1_")).arg(i);
    while (settings.contains(key + QLatin1String("url"))) {
        QUrl url = settings.value(key + QLatin1String("url")).toUrl();
        QString fileName = settings.value(key + QLatin1String("location")).toString();
        bool done = settings.value(key + QLatin1String("done"), true).toBool();
        if (!url.isEmpty() && !fileName.isEmpty()) {
            DownloadItem *item = new DownloadItem(0, this);
            item->mOutput.setFileName(fileName);
            item->fileNameLabel->setText(QFileInfo(item->mOutput.fileName()).fileName());
            item->mUrl = url;
            item->stopButton->setVisible(false);
            item->stopButton->setEnabled(false);
            item->progressBar->setVisible(false);
            if ( QFileInfo( fileName ).suffix() == "tbz" )
            {
                item->nsaButton->setVisible( true );
            }
            addItem(item);
        }
        key = QString(QLatin1String("download_%1_")).arg(++i);
    }
    cleanupButton->setEnabled( mDownloads.count() - activeDownloads() > 0);
    updateActiveItemCount();
}

void DownloadManager::cleanup()
{
    if (mDownloads.isEmpty())
        return;
    mModel->removeRows(0, mDownloads.count());
    updateItemCount();
    updateActiveItemCount();
    if (mDownloads.isEmpty() && mIconProvider) {
        delete mIconProvider;
        mIconProvider = 0;
    }
    mAutoSaver->changeOccurred();
}

void DownloadManager::updateItemCount()
{
    int count = mDownloads.count();
    itemCount->setText(tr("%n Download(s)", "", count));
}

void DownloadManager::setDownloadDirectory(const QString &directory)
{
    mDownloadDir = directory;
    if (!mDownloadDir.isEmpty())
        mDownloadDir += QLatin1Char('/');
}

QString DownloadManager::downloadDirectory()
{
    return mDownloadDir;
}

QString DownloadManager::timeString(double timeRemaining)
{
    QString remaining;

    if (timeRemaining > 60) {
        timeRemaining = timeRemaining / 60;
        timeRemaining = floor(timeRemaining);
        remaining = tr("%n minutes remaining", "", int(timeRemaining));
    }
    else {
        timeRemaining = floor(timeRemaining);
        remaining = tr("%n seconds remaining", "", int(timeRemaining));
    }

    return remaining;
}

QString DownloadManager::dataString(qint64 size)
{
    QString unit;
    double newSize;

    if (size < 1024) {
        newSize = size;
        unit = tr("bytes");
    } else if (size < 1024 * 1024) {
        newSize = (double)size / (double)1024;
        unit = tr("kB");
    } else if (size < 1024 * 1024 * 1024) {
        newSize = (double)size / (double)(1024 * 1024);
        unit = tr("MB");
    } else {
        newSize = (double)size / (double)(1024 * 1024 * 1024);
        unit = tr("GB");
    }

    return QString(QLatin1String("%1 %2")).arg(newSize, 0, 'f', 1).arg(unit);
}

DownloadModel::DownloadModel(DownloadManager *downloadManager, QObject *parent)
    : QAbstractListModel(parent)
    , mDownloadManager(downloadManager)
{
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    if (role == Qt::ToolTipRole)
        if (!mDownloadManager->mDownloads.at(index.row())->downloadedSuccessfully())
            return mDownloadManager->mDownloads.at(index.row())->downloadInfoLabel->text();
    return QVariant();
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : mDownloadManager->mDownloads.count();
}

bool DownloadModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    int lastRow = row + count - 1;
    for (int i = lastRow; i >= row; --i) {
        if (mDownloadManager->mDownloads.at(i)->downloadedSuccessfully() )
        {
            beginRemoveRows(parent, i, i);
            mDownloadManager->mDownloads.takeAt(i)->deleteLater();
            endRemoveRows();
        }
    }
    mDownloadManager->mAutoSaver->changeOccurred();
    mDownloadManager->updateItemCount();
    return true;
}

Qt::ItemFlags DownloadModel::flags(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return 0;

    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    DownloadItem *item = mDownloadManager->mDownloads.at(index.row());
    if (item->downloadedSuccessfully())
        return defaultFlags | Qt::ItemIsDragEnabled;

    return defaultFlags;
}

QMimeData *DownloadModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;
    foreach (const QModelIndex &index, indexes) {
        if (!index.isValid())
            continue;
        DownloadItem *item = mDownloadManager->mDownloads.at(index.row());
        urls.append(QUrl::fromLocalFile(QFileInfo(item->mOutput).absoluteFilePath()));
    }
    mimeData->setUrls(urls);
    return mimeData;
}

#include "downloadmanager.moc"

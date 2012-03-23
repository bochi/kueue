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

#include "configdialog.h"
#include "settings.h"
#include "network.h"
#include "config.h"
#include "kueue.h"
#include "simplecrypt/simplecrypt.h"

#include <QDir>
#include <QDebug>
#include <QFileDialog>


ConfigDialog::ConfigDialog( QWidget *parent )
            : Ui_ConfigDialog()
{
    (void)parent;

    setupUi( this );
   
    connect( cfg_unityEnabled, SIGNAL( toggled( bool ) ), 
             this, SLOT( toggleUnity(bool)) );
    connect( cfg_externalEditorEnabled, SIGNAL( toggled( bool ) ), 
             this, SLOT( toggleUnityEditor( bool ) ) );
    connect( cfg_useIdleTimeout, SIGNAL( toggled( bool ) ),
             this, SLOT( toggleUnityTimeout( bool ) ) );
    connect( filemanagerButton, SIGNAL( pressed() ),
             this, SLOT( getFilemanagerCommand() ) );
             
    connect( editorCommandButton, SIGNAL( pressed() ), 
             this, SLOT( getExternalEditorFile() ) );
    connect( editorSaveLocationButton, SIGNAL( pressed() ), 
             this, SLOT( getEditorSaveLocation() ) );
    connect( downloadDirectoryButton, SIGNAL( pressed() ),
             this, SLOT( getDownloadDirectory() ) );
    connect( buttonBox, SIGNAL( accepted() ), 
             this, SLOT( writeSettings() ) );
    connect( buttonBox, SIGNAL( rejected() ), 
             this, SLOT( close() ) );
    connect( engineerAddButton, SIGNAL( pressed() ), 
             this, SLOT( addEngineer() ) );
    connect( engineerRemoveButton, SIGNAL( pressed() ), 
             this, SLOT( removeEngineer() ) );
    connect( engineerMoveUpButton, SIGNAL( pressed() ),
             this, SLOT( moveEngineerUp() ) );
    connect( engineerMoveDownButton, SIGNAL( pressed() ),
             this, SLOT( moveEngineerDown() ) );
    connect( queueAddButton, SIGNAL( pressed() ), 
             this, SLOT( addQueue() ) );
    connect( queueRemoveButton, SIGNAL( pressed() ), 
             this, SLOT( removeQueue() ) );
    connect( queueMoveUpButton, SIGNAL( pressed() ),
             this, SLOT( moveQueueUp() ) );
    connect( queueMoveDownButton, SIGNAL( pressed() ), 
             this, SLOT( moveQueueDown() ) );
    connect( generalNotificationSoundButton, SIGNAL( pressed() ),
             this, SLOT( getGeneralNotificationSoundFile() ) );
    connect( generalNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playGeneralNotificationSound() ) );
    connect( generalNotificationFileButton, SIGNAL( pressed() ), 
             this, SLOT( getGeneralNotificationWriteFile() ) );
    connect( newPersonalNotificationSoundButton, SIGNAL( pressed() ),
             this, SLOT( getNewPersonalNotificationSoundFile() ) );
    connect( newPersonalNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playNewPersonalNotificationSound() ) );
    connect( newPersonalNotificationFileButton, SIGNAL( pressed() ), 
             this, SLOT( getNewPersonalNotificationWriteFile() ) );
    connect( updatePersonalNotificationSoundButton, SIGNAL( pressed() ), 
             this, SLOT( getUpdatePersonalNotificationSoundFile() ) );
    connect( updatePersonalNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playUpdatePersonalNotificationSound() ) );
    connect( updatePersonalNotificationFileButton, SIGNAL( pressed() ), 
             this, SLOT( getUpdatePersonalNotificationWriteFile() ) );
    connect( bomgarNotificationSoundButton, SIGNAL( pressed() ), 
             this, SLOT( getBomgarNotificationSoundFile() ) );
    connect( bomgarNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playBomgarNotificationSound() ) );
    connect( bomgarNotificationFileButton, SIGNAL( pressed() ),
             this, SLOT( getBomgarNotificationWriteFile() ) );
    connect( lowNotificationSoundButton, SIGNAL( pressed() ), 
             this, SLOT( getLowNotificationSoundFile() ) );
    connect( lowNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playLowNotificationSound() ) );
    connect( lowNotificationFileButton, SIGNAL( pressed() ),
             this, SLOT( getLowNotificationWriteFile() ) );
    connect( mediumNotificationSoundButton, SIGNAL( pressed() ),
             this, SLOT( getMediumNotificationSoundFile() ) );
    connect( mediumNotificationPlayButton, SIGNAL( pressed() ), 
             this, SLOT( playMediumNotificationSound() ) );
    connect( mediumNotificationFileButton, SIGNAL( pressed() ),
             this, SLOT( getMediumNotificationWriteFile() ) );
    connect( urgentNotificationSoundButton, SIGNAL( pressed() ),
             this, SLOT( getUrgentNotificationSoundFile() ) );
    connect( urgentNotificationPlayButton, SIGNAL( pressed() ),
             this, SLOT( playUrgentNotificationSound() ) );
    connect( urgentNotificationFileButton, SIGNAL( pressed() ),
             this, SLOT( getUrgentNotificationWriteFile() ) );
    connect( highNotificationSoundButton, SIGNAL( pressed() ),
             this, SLOT( getHighNotificationSoundFile() ) );
    connect( highNotificationPlayButton, SIGNAL( pressed() ),
             this, SLOT( playHighNotificationSound() ) );
    connect( highNotificationFileButton, SIGNAL( pressed() ),
             this, SLOT( getHighNotificationWriteFile() ) );
    connect( cfg_showSystemTray, SIGNAL( toggled( bool ) ), 
             this, SLOT( toggleSystemTray( bool ) ) );
    connect( cfg_monitorEnabled, SIGNAL( toggled( bool ) ),
             this, SLOT( toggleMonitor( bool ) ) );
    connect( cfg_qbossFeatures, SIGNAL( toggled( bool ) ),
             this, SLOT( toggleQboss( bool ) ) );
    connect( cfg_notificationsEnabled, SIGNAL( toggled( bool ) ), 
             this, SLOT( toggleNotifications( bool ) ) );
    
    connect( cfg_generalNotificationSound, SIGNAL( toggled( bool ) ), 
             cfg_generalNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_generalNotificationSound, SIGNAL( toggled( bool ) ), 
             generalNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_generalNotificationSound, SIGNAL( toggled( bool ) ),
             generalNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_generalNotificationWrite, SIGNAL( toggled( bool ) ), 
             cfg_generalNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_generalNotificationWrite, SIGNAL( toggled( bool ) ),
             generalNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_newPersonalNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_newPersonalNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_newPersonalNotificationSound, SIGNAL( toggled( bool ) ),
             newPersonalNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_newPersonalNotificationSound, SIGNAL( toggled( bool ) ),
             newPersonalNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_newPersonalNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_newPersonalNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_newPersonalNotificationWrite, SIGNAL( toggled( bool ) ),
             newPersonalNotificationFileButton, SLOT( setEnabled( bool ) ) );

    connect( cfg_updatePersonalNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_updatePersonalNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_updatePersonalNotificationSound, SIGNAL( toggled( bool ) ),
             updatePersonalNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_updatePersonalNotificationSound, SIGNAL( toggled( bool ) ), 
             updatePersonalNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_updatePersonalNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_updatePersonalNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_updatePersonalNotificationWrite, SIGNAL( toggled( bool ) ),
             updatePersonalNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_bomgarNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_bomgarNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_bomgarNotificationSound, SIGNAL( toggled( bool ) ),
             bomgarNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_bomgarNotificationSound, SIGNAL( toggled( bool ) ), 
             bomgarNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_bomgarNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_bomgarNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_bomgarNotificationWrite, SIGNAL( toggled( bool ) ), 
             bomgarNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_lowNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_lowNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_lowNotificationSound, SIGNAL( toggled( bool ) ),
             lowNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_lowNotificationSound, SIGNAL( toggled( bool ) ), 
             lowNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_lowNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_lowNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_lowNotificationWrite, SIGNAL( toggled( bool ) ), 
             lowNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_mediumNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_mediumNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_mediumNotificationSound, SIGNAL( toggled( bool ) ),
             mediumNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_mediumNotificationSound, SIGNAL( toggled( bool ) ),
             mediumNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_mediumNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_mediumNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_mediumNotificationWrite, SIGNAL( toggled( bool ) ), 
             mediumNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_urgentNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_urgentNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_urgentNotificationSound, SIGNAL( toggled( bool ) ),
             urgentNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_urgentNotificationSound, SIGNAL( toggled( bool ) ),
             urgentNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_urgentNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_urgentNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_urgentNotificationWrite, SIGNAL( toggled( bool ) ),
             urgentNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    connect( cfg_highNotificationSound, SIGNAL( toggled( bool ) ),
             cfg_highNotificationSoundFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_highNotificationSound, SIGNAL( toggled( bool ) ),
             highNotificationSoundButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_highNotificationSound, SIGNAL( toggled( bool ) ), 
             highNotificationPlayButton, SLOT( setEnabled( bool ) ) );
    connect( cfg_highNotificationWrite, SIGNAL( toggled( bool ) ),
             cfg_highNotificationWriteFile, SLOT( setEnabled( bool ) ) );
    connect( cfg_highNotificationWrite, SIGNAL( toggled( bool ) ),
             highNotificationFileButton, SLOT( setEnabled( bool ) ) );
    
    cfg_dBServer->setText( Settings::dBServer() );
    cfg_engineer->setText( Settings::engineer() );
    

    
    cfg_showAppWindow->setChecked( Settings::showAppWindow() );
    cfg_showTabsAtTop->setChecked( Settings::showTabsAtTop() );
    cfg_showSystemTray->setChecked( Settings::showSystemTray() );
    cfg_animateQueue->setChecked( Settings::animateQueue() );
    cfg_animateQmon->setChecked( Settings::animateQmon() );
    cfg_leftMouseButton->setCurrentIndex( Settings::leftMouseButton() );
    cfg_middleMouseButton->setCurrentIndex( Settings::middleMouseButton() );
    cfg_rightMouseButton->setCurrentIndex( Settings::rightMouseButton() );
    
    cfg_monitorEnabled->setChecked( Settings::monitorEnabled() );
    cfg_queuesToMonitor->addItems( Settings::queuesToMonitor() );
    cfg_monitorPersonalBomgar->setChecked( Settings::monitorPersonalBomgar() );
    cfg_bomgarName->setText( Settings::bomgarName() );
    
    cfg_qbossFeatures->setChecked( Settings::qbossFeatures() );
    cfg_checkKopete->setChecked( Settings::checkKopete() );
    cfg_kopeteText->setText( Settings::kopeteText() );
    cfg_engineerList->addItems( Settings::engineerList() );
    
    cfg_statsEnabled->setChecked( Settings::statsEnabled() );
    
    cfg_unityEnabled->setChecked( Settings::unityEnabled() );
    cfg_otherFileManagerCommand->setText( Settings::otherFileManagerCommand() );
    cfg_showDownloadManager->setChecked( Settings::showDownloadManager() );
    
    cfg_unityPassword->setText( Settings::unityPassword() );
    cfg_unityURL->setText( Settings::unityURL() );
    cfg_useIdleTimeout->setChecked( Settings::useIdleTimeout() );
    cfg_idleTimeoutMinutes->setValue( Settings::idleTimeoutMinutes() );
    cfg_minimumFontSize->setValue( Settings::minimumFontSize() );
    cfg_defaultFontSize->setValue( Settings::defaultFontSize() );
    cfg_externalEditorEnabled->setChecked( Settings::externalEditorEnabled() );
    cfg_editorCommand->setText( Settings::editorCommand() );
    cfg_editorSaveLocation->setText( Settings::editorSaveLocation() );
    cfg_downloadDirectory->setText( Settings::downloadDirectory() );
    cfg_useSrDirectory->setChecked( Settings::useSrDirectory() );
    cfg_autoExtract->setChecked( Settings::autoExtract() );
    
    cfg_notificationsEnabled->setChecked( !Settings::notificationsEnabled() );

    cfg_generalNotificationPopup->setChecked( Settings::generalNotificationPopup() );
    cfg_generalNotificationSound->setChecked( Settings::generalNotificationSound() );
    cfg_generalNotificationSoundFile->setText( Settings::generalNotificationSoundFile() );
    cfg_generalNotificationWrite->setChecked( Settings::generalNotificationWrite() );
    cfg_generalNotificationWriteFile->setText( Settings::generalNotificationWriteFile() );

    cfg_newPersonalNotificationPopup->setChecked( Settings::newPersonalNotificationPopup() );
    cfg_newPersonalNotificationSound->setChecked( Settings::newPersonalNotificationSound() );
    cfg_newPersonalNotificationSoundFile->setText( Settings::newPersonalNotificationSoundFile() );
    cfg_newPersonalNotificationWrite->setChecked( Settings::newPersonalNotificationWrite() );
    cfg_newPersonalNotificationWriteFile->setText( Settings::newPersonalNotificationWriteFile() );

    cfg_updatePersonalNotificationPopup->setChecked( Settings::updatePersonalNotificationPopup() );
    cfg_updatePersonalNotificationSound->setChecked( Settings::updatePersonalNotificationSound() );
    cfg_updatePersonalNotificationSoundFile->setText( Settings::updatePersonalNotificationSoundFile() );
    cfg_updatePersonalNotificationWrite->setChecked( Settings::updatePersonalNotificationWrite() );
    cfg_updatePersonalNotificationWriteFile->setText( Settings::updatePersonalNotificationWriteFile() );

    cfg_bomgarNotificationPopup->setChecked( Settings::bomgarNotificationPopup() );
    cfg_bomgarNotificationSound->setChecked( Settings::bomgarNotificationSound() );
    cfg_bomgarNotificationSoundFile->setText( Settings::bomgarNotificationSoundFile() );
    cfg_bomgarNotificationWrite->setChecked( Settings::bomgarNotificationWrite() );
    cfg_bomgarNotificationWriteFile->setText( Settings::bomgarNotificationWriteFile() );

    cfg_lowNotificationPopup->setChecked( Settings::lowNotificationPopup() );
    cfg_lowNotificationSound->setChecked( Settings::lowNotificationSound() );
    cfg_lowNotificationSoundFile->setText( Settings::lowNotificationSoundFile() );
    cfg_lowNotificationWrite->setChecked( Settings::lowNotificationWrite() );
    cfg_lowNotificationWriteFile->setText( Settings::lowNotificationWriteFile() );

    cfg_mediumNotificationPopup->setChecked( Settings::mediumNotificationPopup() );
    cfg_mediumNotificationSound->setChecked( Settings::mediumNotificationSound() );
    cfg_mediumNotificationSoundFile->setText( Settings::mediumNotificationSoundFile() );
    cfg_mediumNotificationWrite->setChecked( Settings::mediumNotificationWrite() );
    cfg_mediumNotificationWriteFile->setText( Settings::mediumNotificationWriteFile() );

    cfg_urgentNotificationPopup->setChecked( Settings::urgentNotificationPopup() );
    cfg_urgentNotificationSound->setChecked( Settings::urgentNotificationSound() );
    cfg_urgentNotificationSoundFile->setText( Settings::urgentNotificationSoundFile() );
    cfg_urgentNotificationWrite->setChecked( Settings::urgentNotificationWrite() );
    cfg_urgentNotificationWriteFile->setText( Settings::urgentNotificationWriteFile() );

    cfg_highNotificationPopup->setChecked( Settings::highNotificationPopup() );
    cfg_highNotificationSound->setChecked( Settings::highNotificationSound() );
    cfg_highNotificationSoundFile->setText( Settings::highNotificationSoundFile() );
    cfg_highNotificationWrite->setChecked( Settings::highNotificationWrite() );
    cfg_highNotificationWriteFile->setText( Settings::highNotificationWriteFile() );
    
    cfg_generalNotificationSoundFile->setEnabled( Settings::generalNotificationSound() );
    generalNotificationSoundButton->setEnabled( Settings::generalNotificationSound() );
    generalNotificationPlayButton->setEnabled( Settings::generalNotificationSound() );
    cfg_generalNotificationWriteFile->setEnabled( Settings::generalNotificationWrite() );
    generalNotificationFileButton->setEnabled( Settings::generalNotificationWrite() );
    
    cfg_newPersonalNotificationSoundFile->setEnabled( Settings::newPersonalNotificationSound() );
    newPersonalNotificationSoundButton->setEnabled( Settings::newPersonalNotificationSound() );
    newPersonalNotificationPlayButton->setEnabled( Settings::newPersonalNotificationSound() );
    cfg_newPersonalNotificationWriteFile->setEnabled( Settings::newPersonalNotificationWrite() );
    newPersonalNotificationFileButton->setEnabled( Settings::newPersonalNotificationWrite() );
    
    cfg_updatePersonalNotificationSoundFile->setEnabled( Settings::updatePersonalNotificationSound() );
    updatePersonalNotificationSoundButton->setEnabled( Settings::updatePersonalNotificationSound() );
    updatePersonalNotificationPlayButton->setEnabled( Settings::updatePersonalNotificationSound() );
    cfg_updatePersonalNotificationWriteFile->setEnabled( Settings::updatePersonalNotificationWrite() );
    updatePersonalNotificationFileButton->setEnabled( Settings::updatePersonalNotificationWrite() );
    
    cfg_bomgarNotificationSoundFile->setEnabled( Settings::bomgarNotificationSound() );
    bomgarNotificationSoundButton->setEnabled( Settings::bomgarNotificationSound() );
    bomgarNotificationPlayButton->setEnabled( Settings::bomgarNotificationSound() );
    cfg_bomgarNotificationWriteFile->setEnabled( Settings::bomgarNotificationWrite() );
    bomgarNotificationFileButton->setEnabled( Settings::bomgarNotificationWrite() );
    
    cfg_lowNotificationSoundFile->setEnabled( Settings::lowNotificationSound() );
    lowNotificationSoundButton->setEnabled( Settings::lowNotificationSound() );
    lowNotificationPlayButton->setEnabled( Settings::lowNotificationSound() );
    cfg_lowNotificationWriteFile->setEnabled( Settings::lowNotificationWrite() );
    lowNotificationFileButton->setEnabled( Settings::lowNotificationWrite() );
    
    cfg_mediumNotificationSoundFile->setEnabled( Settings::mediumNotificationSound() );
    mediumNotificationSoundButton->setEnabled( Settings::mediumNotificationSound() );
    mediumNotificationPlayButton->setEnabled( Settings::mediumNotificationSound() );
    cfg_mediumNotificationWriteFile->setEnabled( Settings::mediumNotificationWrite() );
    mediumNotificationFileButton->setEnabled( Settings::mediumNotificationWrite() );
    
    cfg_urgentNotificationSoundFile->setEnabled( Settings::urgentNotificationSound() );
    urgentNotificationSoundButton->setEnabled( Settings::urgentNotificationSound() );
    urgentNotificationPlayButton->setEnabled( Settings::urgentNotificationSound() );
    cfg_urgentNotificationWriteFile->setEnabled( Settings::urgentNotificationWrite() );
    urgentNotificationFileButton->setEnabled( Settings::urgentNotificationWrite() );
    
    cfg_highNotificationSoundFile->setEnabled( Settings::highNotificationSound() );
    highNotificationSoundButton->setEnabled( Settings::highNotificationSound() );
    highNotificationPlayButton->setEnabled( Settings::highNotificationSound() );
    cfg_highNotificationWriteFile->setEnabled( Settings::highNotificationWrite() );
    highNotificationFileButton->setEnabled( Settings::highNotificationWrite() );

    toggleSystemTray( Settings::showSystemTray() );
    toggleMonitor( Settings::monitorEnabled() );
    toggleQboss( Settings::qbossFeatures() );
    toggleNotifications( !Settings::notificationsEnabled() );
    toggleUnity( Settings::unityEnabled() );
    
    #ifndef QT_HAS_DBUS
    
        cfg_checkKopete->setVisible( false );
        cfg_kopeteText->setVisible( false );
    
    #endif
        
    #ifndef USE_PHONON
        
        cfg_highNotificationSound->setVisible( false );
        cfg_highNotificationSoundFile->setVisible( false );
        highNotificationSoundButton->setVisible( false );
        highNotificationPlayButton->setVisible( false );
        
        cfg_urgentNotificationSound->setVisible( false );
        cfg_urgentNotificationSoundFile->setVisible( false );
        urgentNotificationSoundButton->setVisible( false );
        urgentNotificationPlayButton->setVisible( false );
        
        cfg_mediumNotificationSound->setVisible( false );
        cfg_mediumNotificationSoundFile->setVisible( false );
        mediumNotificationSoundButton->setVisible( false );
        mediumNotificationPlayButton->setVisible( false );
        
        cfg_lowNotificationSound->setVisible( false );
        cfg_lowNotificationSoundFile->setVisible( false );
        lowNotificationSoundButton->setVisible( false );
        lowNotificationPlayButton->setVisible( false );
        
        cfg_bomgarNotificationSound->setVisible( false );
        cfg_bomgarNotificationSoundFile->setVisible( false );
        bomgarNotificationSoundButton->setVisible( false );
        bomgarNotificationPlayButton->setVisible( false );
        
        cfg_updatePersonalNotificationSound->setVisible( false );
        cfg_updatePersonalNotificationSoundFile->setVisible( false );
        updatePersonalNotificationSoundButton->setVisible( false );
        updatePersonalNotificationPlayButton->setVisible( false );
        
        cfg_newPersonalNotificationSound->setVisible( false );
        cfg_newPersonalNotificationSoundFile->setVisible( false );
        newPersonalNotificationSoundButton->setVisible( false );
        newPersonalNotificationPlayButton->setVisible( false );
        
        cfg_generalNotificationSound->setVisible( false );
        cfg_generalNotificationSoundFile->setVisible( false );
        generalNotificationSoundButton->setVisible( false );
        generalNotificationPlayButton->setVisible( false );
        
    #endif
    
    getQueueList();
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::writeSettings()
{
    Settings::setdBServer( cfg_dBServer->text() );
    Settings::setEngineer( cfg_engineer->text() );
    Settings::setUnityPassword( cfg_unityPassword->text() );
    Settings::setShowDownloadManager( cfg_showDownloadManager->isChecked() );
    Settings::setShowAppWindow( cfg_showAppWindow->isChecked() );
    Settings::setShowTabsAtTop( cfg_showTabsAtTop->isChecked() );
    Settings::setUnityEnabled( cfg_unityEnabled->isChecked() );
    
    if ( cfg_otherFileManagerCommand->text() == "default" )
    {
        Settings::setUseDefaultFileManager( true );
    }
    else
    {
        Settings::setUseDefaultFileManager( false );
    }
    
    Settings::setOtherFileManagerCommand( cfg_otherFileManagerCommand->text() );
    Settings::setUnityURL( cfg_unityURL->text() );
    Settings::setUseIdleTimeout( cfg_useIdleTimeout->isChecked() );
    Settings::setIdleTimeoutMinutes( cfg_idleTimeoutMinutes->value() );
    Settings::setDownloadDirectory( cfg_downloadDirectory->text() );
    Settings::setMinimumFontSize( cfg_minimumFontSize->value() );
    Settings::setDefaultFontSize( cfg_defaultFontSize->value() );
    Settings::setExternalEditorEnabled( cfg_externalEditorEnabled->isChecked() );
    Settings::setEditorCommand( cfg_editorCommand->text() );
    Settings::setEditorSaveLocation( cfg_editorSaveLocation->text() );
    Settings::setShowSystemTray( cfg_showSystemTray->isChecked() );
    Settings::setAnimateQueue( cfg_animateQueue->isChecked() );
    Settings::setAnimateQmon( cfg_animateQmon->isChecked() );
    Settings::setLeftMouseButton( cfg_leftMouseButton->currentIndex() );
    Settings::setMiddleMouseButton( cfg_middleMouseButton->currentIndex() );
    Settings::setRightMouseButton( cfg_rightMouseButton->currentIndex() );
    Settings::setAutoExtract( cfg_autoExtract->isChecked() );
    Settings::setUseSrDirectory( cfg_useSrDirectory->isChecked() );
    Settings::setMonitorEnabled( cfg_monitorEnabled->isChecked() );
    
    QStringList ql;
    
    for ( int i = 0; i < cfg_queuesToMonitor->count(); ++i ) 
    {
        ql.append( cfg_queuesToMonitor->item( i )->text() );
    }
    
    Settings::setQueuesToMonitor( ql );

    Settings::setMonitorPersonalBomgar( cfg_monitorPersonalBomgar->isChecked() );
    Settings::setBomgarName( cfg_bomgarName->text() );
    
    Settings::setQbossFeatures( cfg_qbossFeatures->isChecked() );
    Settings::setCheckKopete( cfg_checkKopete->isChecked() );
    Settings::setKopeteText( cfg_kopeteText->text() );
    
    QStringList el;
    
    for ( int i = 0; i < cfg_engineerList->count(); ++i ) 
    {
        el.append( cfg_engineerList->item( i )->text() );
    }
    
    Settings::setEngineerList( el );
    
    Settings::setStatsEnabled( cfg_statsEnabled->isChecked() );
        
    Settings::setNotificationsEnabled( cfg_notificationsEnabled->isChecked() );
    
    Settings::setGeneralNotificationPopup( cfg_generalNotificationPopup->isChecked() );
    Settings::setGeneralNotificationSound( cfg_generalNotificationSound->isChecked() );
    Settings::setGeneralNotificationSoundFile( cfg_generalNotificationSoundFile->text() );
    Settings::setGeneralNotificationWrite( cfg_generalNotificationWrite->isChecked() );
    Settings::setGeneralNotificationWriteFile( cfg_generalNotificationWriteFile->text() );
    
    Settings::setNewPersonalNotificationPopup( cfg_newPersonalNotificationPopup->isChecked() );
    Settings::setNewPersonalNotificationSound( cfg_newPersonalNotificationSound->isChecked() );
    Settings::setNewPersonalNotificationSoundFile( cfg_newPersonalNotificationSoundFile->text() );
    Settings::setNewPersonalNotificationWrite( cfg_newPersonalNotificationWrite->isChecked() );
    Settings::setNewPersonalNotificationWriteFile( cfg_newPersonalNotificationWriteFile->text() );
    
    Settings::setUpdatePersonalNotificationPopup( cfg_updatePersonalNotificationPopup->isChecked() );
    Settings::setUpdatePersonalNotificationSound( cfg_updatePersonalNotificationSound->isChecked() );
    Settings::setUpdatePersonalNotificationSoundFile( cfg_updatePersonalNotificationSoundFile->text() );
    Settings::setUpdatePersonalNotificationWrite( cfg_updatePersonalNotificationWrite->isChecked() );
    Settings::setUpdatePersonalNotificationWriteFile( cfg_updatePersonalNotificationWriteFile->text() );
    
    Settings::setBomgarNotificationPopup( cfg_bomgarNotificationPopup->isChecked() );
    Settings::setBomgarNotificationSound( cfg_bomgarNotificationSound->isChecked() );
    Settings::setBomgarNotificationSoundFile( cfg_bomgarNotificationSoundFile->text() );
    Settings::setBomgarNotificationWrite( cfg_bomgarNotificationWrite->isChecked() );
    Settings::setBomgarNotificationWriteFile( cfg_bomgarNotificationWriteFile->text() );
    
    Settings::setLowNotificationPopup( cfg_lowNotificationPopup->isChecked() );
    Settings::setLowNotificationSound( cfg_lowNotificationSound->isChecked() );
    Settings::setLowNotificationSoundFile( cfg_lowNotificationSoundFile->text() );
    Settings::setLowNotificationWrite( cfg_lowNotificationWrite->isChecked() );
    Settings::setLowNotificationWriteFile( cfg_lowNotificationWriteFile->text() );
    
    Settings::setMediumNotificationPopup( cfg_mediumNotificationPopup->isChecked() );
    Settings::setMediumNotificationSound( cfg_mediumNotificationSound->isChecked() );
    Settings::setMediumNotificationSoundFile( cfg_mediumNotificationSoundFile->text() );
    Settings::setMediumNotificationWrite( cfg_mediumNotificationWrite->isChecked() );
    Settings::setMediumNotificationWriteFile( cfg_mediumNotificationWriteFile->text() );
    
    Settings::setUrgentNotificationPopup( cfg_urgentNotificationPopup->isChecked() );
    Settings::setUrgentNotificationSound( cfg_urgentNotificationSound->isChecked() );
    Settings::setUrgentNotificationSoundFile( cfg_urgentNotificationSoundFile->text() );
    Settings::setUrgentNotificationWrite( cfg_urgentNotificationWrite->isChecked() );
    Settings::setUrgentNotificationWriteFile( cfg_urgentNotificationWriteFile->text() );
    
    Settings::setHighNotificationPopup( cfg_highNotificationPopup->isChecked() );
    Settings::setHighNotificationSound( cfg_highNotificationSound->isChecked() );
    Settings::setHighNotificationSoundFile( cfg_highNotificationSoundFile->text() );
    Settings::setHighNotificationWrite( cfg_highNotificationWrite->isChecked() );
    Settings::setHighNotificationWriteFile( cfg_highNotificationWriteFile->text() );
    
    emit settingsChanged();
}

void ConfigDialog::getQueueList()
{
    mQueueReply = Network::get( "pseudoQ" );
    connect( mQueueReply, SIGNAL( finished() ), this, SLOT( queueJobDone() ) );
}

void ConfigDialog::queueJobDone()
{
    QString data = mQueueReply->readAll();
    
    if ( !mQueueReply->error() )
    {
        QStringList list = data.split( "\n" );
        
        list.sort();
        siebelCombo->addItems( list );
    }
    else
    {
        Kueue::notify( "kueue-general", "Error", "<b>Fetching Siebel queues failed.</b><br>No VPN connection or networking issues?", "" );
    }
}

void ConfigDialog::addEngineer()
{
    if ( !engineerLineEdit->text().isEmpty() )
    {
        cfg_engineerList->addItem( engineerLineEdit->text() );
        engineerLineEdit->clear();
    }
}

void ConfigDialog::removeEngineer()
{
    for ( int i = 0; i < cfg_engineerList->selectedItems().count(); ++i )
    {
        delete cfg_engineerList->item( cfg_engineerList->currentRow() );
    }
}

void ConfigDialog::moveEngineerUp()
{
    int row = cfg_engineerList->currentRow();
    cfg_engineerList->insertItem( ( row - 1 ), cfg_engineerList->takeItem( row ) );
    cfg_engineerList->setCurrentRow( row - 1 );
}

void ConfigDialog::moveEngineerDown()
{
    int row = cfg_engineerList->currentRow();
    cfg_engineerList->insertItem( ( row + 1 ), cfg_engineerList->takeItem( row ) );
    cfg_engineerList->setCurrentRow( row + 1 );
}

void ConfigDialog::addQueue()
{
    cfg_queuesToMonitor->addItem( queueLineEdit->text() + "|" + siebelCombo->currentText() );
    engineerLineEdit->clear();
}

void ConfigDialog::removeQueue()
{
    for ( int i = 0; i < cfg_queuesToMonitor->selectedItems().count(); ++i )
    {
        delete cfg_queuesToMonitor->item( cfg_queuesToMonitor->currentRow() );
    }
}

void ConfigDialog::moveQueueUp()
{
    int row = cfg_queuesToMonitor->currentRow();
    cfg_queuesToMonitor->insertItem( ( row - 1 ), cfg_queuesToMonitor->takeItem( row ) );
    cfg_queuesToMonitor->setCurrentRow( row - 1 );
}

void ConfigDialog::moveQueueDown()
{
    int row = cfg_queuesToMonitor->currentRow();
    cfg_queuesToMonitor->insertItem( ( row + 1 ), cfg_queuesToMonitor->takeItem( row ) );
    cfg_queuesToMonitor->setCurrentRow( row + 1 );
}

void ConfigDialog::toggleSystemTray( const bool& b )
{
    cfg_animateQmon->setEnabled( b );
    cfg_animateQueue->setEnabled( b );
}

void ConfigDialog::toggleMonitor( const bool& b )
{
    pageListWidget->item( 1 )->setHidden( !b );
}

void ConfigDialog::toggleQboss( const bool& b )
{
    pageListWidget->item( 2 )->setHidden( !b );
}

void ConfigDialog::toggleNotifications( const bool& b )
{
    notificationSettingsGroupBox->setEnabled( !b );
}

void ConfigDialog::toggleUnity( const bool& b )
{
    pageListWidget->item( 3 )->setHidden( !b );
    
    if ( !Settings::externalEditorEnabled() )
    {
        unityEditorGroupBox->setEnabled( false );
    }
    else
    {
        unityEditorGroupBox->setEnabled( b );
    }
    
    if ( !Settings::useIdleTimeout() )
    {
        toggleUnityTimeout( false );
    }
    else
    {
        toggleUnityTimeout( b );
    }
    
    cfg_useIdleTimeout->setEnabled( b );
}

void ConfigDialog::toggleUnityEditor( const bool& b )
{
    if ( !Settings::unityEnabled() )
    {
        unityEditorGroupBox->setEnabled( false );
    }
    else
    {
        unityEditorGroupBox->setEnabled( b );
    }
}

void ConfigDialog::toggleUnityTimeout( const bool& b )
{
    cfg_idleTimeoutMinutes->setEnabled( b );
    idleTimeoutLabel->setEnabled( b );
}

void ConfigDialog::getGeneralNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_generalNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getGeneralNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_generalNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playGeneralNotificationSound()
{
    Kueue::playSound( cfg_generalNotificationSoundFile->text() );
}

void ConfigDialog::getNewPersonalNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_newPersonalNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getNewPersonalNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_newPersonalNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playNewPersonalNotificationSound()
{
    Kueue::playSound( cfg_newPersonalNotificationSoundFile->text() );
}

void ConfigDialog::getUpdatePersonalNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_updatePersonalNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getUpdatePersonalNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_updatePersonalNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playUpdatePersonalNotificationSound()
{
    Kueue::playSound( cfg_updatePersonalNotificationSoundFile->text() );
}

void ConfigDialog::getBomgarNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_bomgarNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getBomgarNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_bomgarNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playBomgarNotificationSound()
{
    Kueue::playSound( cfg_bomgarNotificationSoundFile->text() );
}

void ConfigDialog::getLowNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_lowNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getLowNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_lowNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playLowNotificationSound()
{
    Kueue::playSound( cfg_lowNotificationSoundFile->text() );
}

void ConfigDialog::getMediumNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_mediumNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getMediumNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_mediumNotificationSoundFile->setText( fileName );
}

void ConfigDialog::playMediumNotificationSound()
{
    Kueue::playSound( cfg_mediumNotificationSoundFile->text() );
}

void ConfigDialog::getUrgentNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_urgentNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getUrgentNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_urgentNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playUrgentNotificationSound()
{
    Kueue::playSound( cfg_urgentNotificationSoundFile->text() );
}

void ConfigDialog::getHighNotificationSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Sound File"), QDir::homePath(), "Sound Files (*.wav *.mp3 *.ogg)" );
    cfg_highNotificationSoundFile->setText( fileName );
}

void ConfigDialog::getHighNotificationWriteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"), QDir::homePath(), "Text Files (*.txt *.log)" );
    cfg_highNotificationWriteFile->setText( fileName );
}

void ConfigDialog::playHighNotificationSound()
{
    Kueue::playSound( cfg_highNotificationSoundFile->text() );
}

void ConfigDialog::getExternalEditorFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select editor"), QDir::homePath() );
    cfg_editorCommand->setText( fileName );
}

void ConfigDialog::getEditorSaveLocation()
{
    QString fileName = QFileDialog::getExistingDirectory( this, tr("Select location"), QDir::homePath() );
    cfg_editorSaveLocation->setText( fileName );
}

void ConfigDialog::getDownloadDirectory()
{
    QString fileName = QFileDialog::getExistingDirectory( this, tr("Select location"), QDir::homePath() );
    cfg_downloadDirectory->setText( fileName );
}

void ConfigDialog::getFilemanagerCommand()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select command"), QDir::homePath() );
    
    if ( fileName.isEmpty() || !QFile::exists( fileName ) )
    {
        cfg_otherFileManagerCommand->setText( "default" );
    }
    else
    {
        cfg_otherFileManagerCommand->setText( fileName );
    }
}

BasicConfig::BasicConfig()
{
    qDebug() << "[BASICCONFIG] Constructing";
    
    setupUi( this ); 
    
    if ( !Settings::dBServer().isEmpty() )
    {
        dbServer->setText( Settings::dBServer() );
    }
    else
    {
        dbServer->setText( "http://" );
    }
    
    if ( !Settings::engineer().isEmpty() )
    {
        engineer->setText( Settings::engineer() );
    }
    
    quitButton->setDefault( false );
    saveButton->setDefault( true );
    
    connect( saveButton, SIGNAL( pressed() ), this, SLOT( saveConfig() ) );
}

BasicConfig::~BasicConfig()
{
    qDebug() << "[BASICCONFIG] Destroying";
}

void BasicConfig::saveConfig()
{
    if ( engineer->text().isEmpty()|| dbServer->text().isEmpty() )
    {
        QMessageBox* box = new QMessageBox( this );
        
        box->setText( "For kueue to function, at least a <b>DB Server</b> and an <b>Engineer</b> (ie. your login) are required." );
        box->setWindowTitle( "Error" );
        box->setStandardButtons( QMessageBox::Yes );
        box->setDefaultButton( QMessageBox::Yes );
        box->setIcon( QMessageBox::Information );
        box->exec();
        delete box;
    }
    else
    {
        Settings::setAppVersion( QApplication::applicationVersion() );
        Settings::setdBServer( dbServer->text() );
        Settings::setEngineer( engineer->text() );
        accept();
    }
}


#include "configdialog.moc"

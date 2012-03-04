; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "kueue"
!define PRODUCT_PUBLISHER "Stefan Bogner"
!define PRODUCT_WEB_SITE "http://w3.suse.de/~sbogner/kueue"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\kueue.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "kueue.ico"
!define MUI_UNICON "kueue.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "gpl-v3.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\kueue.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}-setup.exe"
InstallDir "$PROGRAMFILES\kueue"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Hauptgruppe" SEC01
  SetOutPath "$INSTDIR\codecs"
  SetOverwrite try
  File "kueue\codecs\qcncodecs4.dll"
  File "kueue\codecs\qjpcodecs4.dll"
  File "kueue\codecs\qkrcodecs4.dll"
  File "kueue\codecs\qtwcodecs4.dll"
  SetOutPath "$INSTDIR\iconengines"
  File "kueue\iconengines\qsvgicon4.dll"
  SetOutPath "$INSTDIR\imageformats"
  File "kueue\imageformats\qgif4.dll"
  File "kueue\imageformats\qico4.dll"
  File "kueue\imageformats\qjpeg4.dll"
  File "kueue\imageformats\qmng4.dll"
  File "kueue\imageformats\qsvg4.dll"
  File "kueue\imageformats\qtiff4.dll"
  SetOutPath "$INSTDIR"
  File "kueue\kueue.exe"
  CreateDirectory "$SMPROGRAMS\kueue"
  CreateShortCut "$SMPROGRAMS\kueue\kueue.lnk" "$INSTDIR\kueue.exe"
  CreateShortCut "$DESKTOP\kueue.lnk" "$INSTDIR\kueue.exe"
  File "kueue\libgcc_s_dw2-1.dll"
  File "kueue\mingwm10.dll"
  File "kueue\phonon4.dll"
  SetOutPath "$INSTDIR\phonon_backend"
  File "kueue\phonon_backend\libphonon_ds9d4.a"
  File "kueue\phonon_backend\libphonon_ds94.a"
  File "kueue\phonon_backend\phonon_ds9d4.dll"
  File "kueue\phonon_backend\phonon_ds94.dll"
  SetOutPath "$INSTDIR"
  File "kueue\QtCore4.dll"
  File "kueue\QtGui4.dll"
File "kueue\QtXml4.dll"
  File "kueue\libarchive.dll"
File "kueue\zlibwapi.dll"
File "kueue\bzip2.dll"
  File "kueue\QtNetwork4.dll"
  File "kueue\QtOpenGL4.dll"
  File "kueue\QtSql4.dll"
  File "kueue\QtWebKit4.dll"
  File "kueue\libeay32.dll"
  File "kueue\libssl32.dll"
  File "kueue\ssleay32.dll"
  SetOutPath "$INSTDIR\sqldrivers"
  File "kueue\sqldrivers\libqsqlite4.a"
  File "kueue\sqldrivers\qsqlite4.dll"
  File "kueue\sqldrivers\qsqlodbc4.dll"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  CreateShortCut "$SMPROGRAMS\kueue\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\kueue.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\kueue.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) has been successfully uninstalled"
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Would you really like to uninstall $(^Name)?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\sqldrivers\qsqlodbc4.dll"
  Delete "$INSTDIR\sqldrivers\qsqlite4.dll"
  Delete "$INSTDIR\sqldrivers\libqsqlite4.a"
  Delete "$INSTDIR\QtWebKit4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\libssl32.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\phonon_backend\phonon_ds94.dll"
  Delete "$INSTDIR\phonon_backend\phonon_ds9.prl"
  Delete "$INSTDIR\phonon_backend\libphonon_ds94.a"
  Delete "$INSTDIR\phonon_backend\libphonon_ds9.a"
  Delete "$INSTDIR\phonon4.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\kueue.exe"
  Delete "$INSTDIR\imageformats\qtiff4.dll"
  Delete "$INSTDIR\imageformats\qsvg4.dll"
  Delete "$INSTDIR\imageformats\qmng4.dll"
  Delete "$INSTDIR\imageformats\qjpeg4.dll"
  Delete "$INSTDIR\imageformats\qico4.dll"
  Delete "$INSTDIR\imageformats\qgif4.dll"
  Delete "$INSTDIR\iconengines\qsvgicon4.dll"
  Delete "$INSTDIR\codecs\qtwcodecs4.dll"
  Delete "$INSTDIR\codecs\qkrcodecs4.dll"
  Delete "$INSTDIR\codecs\qjpcodecs4.dll"
  Delete "$INSTDIR\codecs\qcncodecs4.dll"

  Delete "$SMPROGRAMS\kueue\Uninstall.lnk"
  Delete "$DESKTOP\kueue.lnk"
  Delete "$SMPROGRAMS\kueue\kueue.lnk"

  RMDir "$SMPROGRAMS\kueue"
  RMDir "$INSTDIR\sqldrivers"
  RMDir "$INSTDIR\phonon_backend"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\iconengines"
  RMDir "$INSTDIR\codecs"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

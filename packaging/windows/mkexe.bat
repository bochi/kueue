@echo off

IF [%1]==[] GOTO MISSING

REM
REM Find out the mingw directory
REM

FOR /F "tokens=*" %%a IN ('where mingw32-gcc.exe') DO ( @SET MGW=%%a )
for %%F in ("%MGW%") do set MINGWDIR=%%~dpF

REM
REM Copy necessary files to folder structure
REM

mkdir kueue
mkdir kueue\codecs
mkdir kueue\sqldrivers
mkdir kueue\phonon_backend
mkdir kueue\imageformats
mkdir kueue\iconengines

copy %QTDIR%\bin\QtCore4.dll kueue
copy %QTDIR%\bin\QtGui4.dll kueue
copy %QTDIR%\bin\QtNetwork4.dll kueue
copy %QTDIR%\bin\QtSql4.dll kueue
copy %QTDIR%\bin\QtOpenGL4.dll kueue
copy %QTDIR%\bin\QtXml4.dll kueue
copy %QTDIR%\bin\QtWebKit4.dll kueue   
copy %QTDIR%\bin\phonon4.dll kueue
copy %MINGWDIR%\mingwm10.dll kueue
copy %MINGWDIR%\libgcc_s_dw2-1.dll kueue

copy C:\OpenSSL\libeay32.dll kueue
copy C:\OpenSSL\libssl32.dll kueue
copy "C:\Program Files\libarchive\bin\libarchive.dll" kueue
copy "C:\Program Files\kueue\*.dll" kueue
copy c:\dev\zlibwapi.dll kueue
copy C:\OpenSSL\ssleay32.dll kueue

copy %QTDIR%\plugins\codecs\qcncodecs4.dll kueue\codecs
copy %QTDIR%\plugins\codecs\qjpcodecs4.dll kueue\codecs
copy %QTDIR%\plugins\codecs\qkrcodecs4.dll kueue\codecs
copy %QTDIR%\plugins\codecs\qtwcodecs4.dll kueue\codecs

copy %QTDIR%\plugins\iconengines\qsvgicon4.dll kueue\iconengines

copy %QTDIR%\plugins\imageformats\qgif4.dll kueue\imageformats
copy %QTDIR%\plugins\imageformats\qico4.dll kueue\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg4.dll kueue\imageformats
copy %QTDIR%\plugins\imageformats\qmng4.dll kueue\imageformats
copy %QTDIR%\plugins\imageformats\qsvg4.dll kueue\imageformats
copy %QTDIR%\plugins\imageformats\qtiff4.dll kueue\imageformats

copy %QTDIR%\plugins\phonon_backend\* kueue\phonon_backend  

copy %QTDIR%\plugins\sqldrivers\libqsqlite4.a kueue\sqldrivers
copy %QTDIR%\plugins\sqldrivers\qsqlite4.dll kueue\sqldrivers
copy %QTDIR%\plugins\sqldrivers\qsqlodbc4.dll kueue\sqldrivers

copy ..\..\build\kueue.exe kueue\kueue.exe

REM
REM The next step assumes that makensis is in %PATH%
REM

makensis /DPRODUCT_VERSION=%1 kueue-setup.nsi
copy /Y kueue-%1-setup.exe %HOMEPATH%
goto END

:MISSING
echo.
echo.
echo. Usage: mkexe.bat VERSION
echo.

:END

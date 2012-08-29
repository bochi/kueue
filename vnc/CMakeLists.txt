#
#               kueue - keep track of your SR queue
#         (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the
#
#    Free Software Foundation, Inc.
#    59 Temple Place - Suite 330
#    Boston, MA  02111-1307, USA
#
#    Have a lot of fun :-)

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

FIND_PACKAGE( Qt4 REQUIRED )
FIND_PACKAGE( LibVNCServer REQUIRED )
FIND_PACKAGE( JPEG REQUIRED )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(  ${QT_INCLUDES} 
                      ${QT_QTXML_LIBRARY}
                      ${CMAKE_CURRENT_BINARY_DIR} 
                      ${CMAKE_CURRENT_SOURCE_DIR} 
		      ${LIBVNCCLIENT_INCLUDE_DIR} )

ADD_DEFINITIONS( ${QT_DEFINITIONS} )

SET( qvnc_CPP  remoteview.cpp  
	       vncclientthread.cpp  
	       vncview.cpp )

QT4_AUTOMOC( ${qvnc_CPP} )

ADD_LIBRARY( qvnc STATIC ${qvnc_CPP} )

TARGET_LINK_LIBRARIES( qvnc ${QT_LIBRARIES} 
                            ${LIBVNCCLIENT_LIBRARIES}
			    ${JPEG_LIBRARIES} ) 



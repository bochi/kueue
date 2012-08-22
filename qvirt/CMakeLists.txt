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

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules )

FIND_PACKAGE( Qt4 REQUIRED )
FIND_PACKAGE( LibVirt REQUIRED )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(  ${QT_INCLUDES} 
                      ${CMAKE_CURRENT_BINARY_DIR} 
                      ${CMAKE_CURRENT_SOURCE_DIR} 
		      ${LIBVIRT_INCLUDE_DIR} )

ADD_DEFINITIONS( ${QT_DEFINITIONS} )

SET( qvirt_CPP  qvirt.cpp)

QT4_AUTOMOC( ${qvirt_CPP} )

ADD_LIBRARY( qvirt STATIC ${qvirt_CPP} )

TARGET_LINK_LIBRARIES( qvirt ${QT_LIBRARIES} 
                             ${LIBVIRT_LIBRARIES} ) 



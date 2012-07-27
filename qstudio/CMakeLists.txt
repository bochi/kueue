#
#               kueue - keep track of your SR queue
#            (C) 2011 Stefan Bogner <sbogner@novell.com>
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

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

SET( QStudio_CPP qstudio.cpp )

QT4_AUTOMOC( ${QStudio_CPP} )

ADD_LIBRARY( qstudio STATIC ${QStudio_CPP} )

TARGET_LINK_LIBRARIES( qstudio ${QT_LIBRARIES} ) 

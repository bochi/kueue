# - Try to find the LibXslt library
# Once done this will define
#
#  LIBXSLT_FOUND - system has LibXslt
#  LIBXSLT_INCLUDE_DIR - the LibXslt include directory
#  LIBXSLT_LIBRARIES - Link these to LibXslt
#  LIBXSLT_DEFINITIONS - Compiler switches required for using LibXslt

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
# Copyright 2006 Alexander Neundorf <neundorf@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_LIBVIRT libvirt)
#SET(LIBXSLT_DEFINITIONS ${PC_LIBXSLT_CFLAGS_OTHER})

FIND_PATH(LIBVIRT_INCLUDE_DIR NAMES libvirt/libvirt.h
    HINTS
    ${PC_LIBVIRT_INCLUDEDIR}
    ${PC_LIBVIRT_INCLUDE_DIRS}
  )

FIND_LIBRARY(LIBVIRT_LIBRARIES NAMES virt libvirt
    HINTS
    ${PC_LIBVIRT_LIBDIR}
    ${PC_LIBVIRT_LIBRARY_DIRS}
  )

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibVirt DEFAULT_MSG LIBVIRT_LIBRARIES LIBVIRT_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBVIRT_INCLUDE_DIR LIBVIRT_LIBRARIES)


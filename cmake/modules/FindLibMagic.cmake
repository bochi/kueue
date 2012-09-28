# - Try to find libmagic header and library
#
# Usage of this module as follows:
#
# find_package(LibMagic)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# LibMagic_ROOT_DIR Set this variable to the root installation of
# libmagic if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# LIBMAGIC_FOUND System has libmagic and magic.h
# LibMagic_LIBRARY The libmagic library
# LibMagic_INCLUDE_DIR The location of magic.h

FIND_PATH(LibMagic_INCLUDE_DIR NAMES magic.h)
MARK_AS_ADVANCED(LibMagic_INCLUDE_DIR)

FIND_LIBRARY(LibMagic_LIBRARY NAMES magic libmagic)
MARK_AS_ADVANCED(LibMagic_LIBRARY)

INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( LibMagic DEFAULT_MSG LibMagic_LIBRARY LibMagic_INCLUDE_DIR )




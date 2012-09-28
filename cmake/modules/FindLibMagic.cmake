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

FIND_PATH(LIBMAGIC_INCLUDE_DIR magic.h)

FIND_LIBRARY(LIBMAGIC_LIBRARY NAMES magic)

IF (LIBMAGIC_INCLUDE_DIR AND LIBMAGIC_LIBRARY)
    SET(LIBMAGIC_FOUND TRUE)
ENDIF (LIBMAGIC_INCLUDE_DIR AND LIBMAGIC_LIBRARY)

IF (LIBMAGIC_FOUND)
    IF (NOT LibMagic_FIND_QUIETLY)
        MESSAGE(STATUS "Found libmagic: ${LIBMAGIC_LIBRARY}")
        MESSAGE(STATUS "      includes: ${LIBMAGIC_INCLUDE_DIR}")
    ENDIF (NOT LibMagic_FIND_QUIETLY)
ELSE (LIBMAGIC_FOUND)
    IF (LibMagic_FIND_REQUIRED)
        MESSAGE(STATUS "")
        MESSAGE(STATUS "libmagic development package cannot be found. Install it, please")
        MESSAGE(STATUS "For example in (open)SUSE it's file-devel package")
        MESSAGE(STATUS "")
        MESSAGE(FATAL_ERROR "Could not find libmagic")
    ENDIF (LibMagic_FIND_REQUIRED)
ENDIF (LIBMAGIC_FOUND)



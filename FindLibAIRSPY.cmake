# - Try to find LibAIRSPY
# Once done this will define
#
#  LibAIRSPY_FOUND - System has libairspy
#  LibAIRSPY_INCLUDE_DIRS - The libairspy include directories
#  LibAIRSPY_LIBRARIES - The libraries needed to use libairspy
#  LibAIRSPY_DEFINITIONS - Compiler switches required for using libairspy
#  LibAIRSPY_VERSION - The librtlsdr version
#

find_package(PkgConfig)
pkg_check_modules(PC_LibAIRSPY libairspy)
set(LibAIRSPY_DEFINITIONS ${PC_LibAIRSPY_CFLAGS_OTHER})

find_path(
    LibAIRSPY_INCLUDE_DIRS
    NAMES libairspy/airspy.h
    HINTS $ENV{LibAIRSPY_DIR}/include
        ${PC_LibAIRSPY_INCLUDEDIR}
    PATHS /usr/local/include
          /usr/include
)

find_library(
    LibAIRSPY_LIBRARIES
    NAMES airspy
    HINTS $ENV{LibAIRSPY_DIR}/lib
        ${PC_LibAIRSPY_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
)

set(LibAIRSPY_VERSION ${PC_LibAIRSPY_VERSION})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LibAIRSPY_FOUND to TRUE
# if all listed variables are TRUE
# Note that `FOUND_VAR LibAIRSPY_FOUND` is needed for cmake 3.2 and older.
find_package_handle_standard_args(LibAIRSPY
                                  FOUND_VAR LibAIRSPY_FOUND
                                  REQUIRED_VARS LibAIRSPY_LIBRARIES LibAIRSPY_INCLUDE_DIRS
                                  VERSION_VAR LibAIRSPY_VERSION)

mark_as_advanced(LibAIRSPY_LIBRARIES LibAIRSPY_INCLUDE_DIRS)

# - Find fftw3
# Find the native FFTW3 headers and libraries.
#
#  FFTW3_INCLUDE_DIRS - where to find asl.h, etc.
#  FFTW3_LIBRARIES    - List of libraries when using fftw3.
#  FFTW3_FOUND        - True if asl found.

# Look for the header file.
FIND_PATH(FFTW3_INCLUDE_DIR NAMES fftw3.h)

# Look for the library.

IF( MSVC)
    FIND_LIBRARY(FFTW3_LIBRARY NAMES fftw3f)
ELSE( MSVC)
    FIND_LIBRARY(FFTW3_LIBRARY NAMES fftw3f)
    FIND_LIBRARY(FFTW3_THREADS_LIBRARY NAMES fftw3f_threads)
ENDIF( MSVC)

# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3 DEFAULT_MSG FFTW3_LIBRARY FFTW3_INCLUDE_DIR)

# Copy the results to the output variables.
IF(FFTW3_FOUND)
  SET(FFTW3_LIBRARIES ${FFTW3_LIBRARY} ${FFTW3_THREADS_LIBRARY})
  SET(FFTW3_INCLUDE_DIRS ${FFTW3_INCLUDE_DIR})
ELSE(FFTW3_FOUND)
  SET(FFTW3_LIBRARIES)
  SET(FFTW3_INCLUDE_DIRS)
ENDIF(FFTW3_FOUND)

#MARK_AS_ADVANCED(FFTW3_INCLUDE_DIR FFTW3_LIBRARY)

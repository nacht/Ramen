#==========
#
# Variables defined by this module:
#   IECOREGL_FOUND    
#   IECOREGL_INCLUDE_DIRS
#   IECOREGL_LIBRARIES
#
# Usage: 
#   FIND_PACKAGE( IECoreGL )
#   FIND_PACKAGE( IECoreGL REQUIRED )
#
#==========

FIND_PATH( IECOREGL_INCLUDE_DIR NAMES IECoreGL/IECoreGL.h)
FIND_LIBRARY( IECOREGL_LIBRARY IECoreGL)

# did we find everything?
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( IECOREGL DEFAULT_MSG IECOREGL_LIBRARY IECOREGL_INCLUDE_DIR)

# Copy the results to the output variables.
IF(IECOREGL_FOUND)
  SET(IECOREGL_LIBRARIES ${IECOREGL_LIBRARY})
  SET(IECOREGL_INCLUDE_DIRS ${IECOREGL_INCLUDE_DIR})
ELSE(IECOREGL_FOUND)
  SET(IECOREGL_LIBRARIES)
  SET(IECOREGL_INCLUDE_DIRS)
ENDIF(IECOREGL_FOUND)

MARK_AS_ADVANCED( IECOREGL_INCLUDE_DIR IECOREGL_LIBRARY)


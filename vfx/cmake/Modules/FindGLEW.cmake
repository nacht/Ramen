# - Find glew
# Find GLEW headers and libraries.
#
#  GLEW_INCLUDE_DIRS - where to find tbb uncludes.
#  GLEW_LIBRARIES    - List of libraries when using tbb.
#  GLEW_FOUND        - True if asl found.

# Look for the header file.
FIND_PATH(GLEW_INCLUDE_DIR NAMES GL/glew.h)

# Look for the library.
FIND_LIBRARY(GLEW_LIBRARY NAMES GLEW)

# handle the QUIETLY and REQUIRED arguments and set GLEW_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG GLEW_LIBRARY GLEW_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GLEW_FOUND)
  SET(GLEW_LIBRARIES ${GLEW_LIBRARY})
  SET(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
ELSE(GLEW_FOUND)
  SET(GLEW_LIBRARIES)
  SET(GLEW_INCLUDE_DIRS)
ENDIF(GLEW_FOUND)

MARK_AS_ADVANCED(GLEW_INCLUDE_DIR GLEW_LIBRARY)

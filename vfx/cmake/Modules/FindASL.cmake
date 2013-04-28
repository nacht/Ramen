# - Find asl
# Find ASL headers and libraries.
#
#  ASL_INCLUDE_DIRS - where to find asl includes.
#  ASL_LIBRARIES    - List of libraries when using asl.
#  ASL_FOUND        - True if asl found.

# Look for the header file.
FIND_PATH(ASL_INCLUDE_DIR NAMES adobe/eve.hpp)

# Look for the library.
FIND_LIBRARY(ASL_LIBRARY NAMES asl)

# handle the QUIETLY and REQUIRED arguments and set ASL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ASL DEFAULT_MSG ASL_LIBRARY ASL_INCLUDE_DIR)

# Copy the results to the output variables.
IF(ASL_FOUND)
  SET(ASL_LIBRARIES ${ASL_LIBRARY})
  SET(ASL_INCLUDE_DIRS ${ASL_INCLUDE_DIR})
ELSE(ASL_FOUND)
  SET(ASL_LIBRARIES)
  SET(ASL_INCLUDE_DIRS)
ENDIF(ASL_FOUND)

MARK_AS_ADVANCED(ASL_INCLUDE_DIR ASL_LIBRARY)


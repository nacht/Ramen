# - Find tbb
# Find the native TBB headers and libraries.
#
#  TBB_INCLUDE_DIRS - where to find tbb uncludes.
#  TBB_LIBRARIES    - List of libraries when using tbb.
#  TBB_FOUND        - True if tbb found.

# Look for the header file.
FIND_PATH(TBB_INCLUDE_DIR NAMES tbb/parallel_for.h)

# Look for the library.
FIND_LIBRARY(TBB_LIBRARY NAMES tbb)

# handle the QUIETLY and REQUIRED arguments and set TBB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TBB DEFAULT_MSG TBB_LIBRARY TBB_INCLUDE_DIR)

# Copy the results to the output variables.
IF(TBB_FOUND)
  SET(TBB_LIBRARIES ${TBB_LIBRARY})
  SET(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
ELSE(TBB_FOUND)
  SET(TBB_LIBRARIES)
  SET(TBB_INCLUDE_DIRS)
ENDIF(TBB_FOUND)

MARK_AS_ADVANCED(TBB_INCLUDE_DIR TBB_LIBRARY TBB_MALLOC_LIBRARY)


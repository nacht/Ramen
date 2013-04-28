# - Find agg
# Find AGG headers and libraries.
#
#  AGG_INCLUDE_DIRS - where to find agg includes.
#  AGG_LIBRARIES    - List of libraries when using agg.
#  AGG_FOUND        - True if agg found.

FIND_PATH(AGG_INCLUDE_DIR NAMES agg/agg_bspline.h)
FIND_LIBRARY(AGG_LIBRARY NAMES agg)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AGG DEFAULT_MSG AGG_LIBRARY AGG_INCLUDE_DIR)

# Copy the results to the output variables.
IF(AGG_FOUND)
  SET(AGG_LIBRARIES ${AGG_LIBRARY})
  SET(AGG_INCLUDE_DIRS ${AGG_INCLUDE_DIR})
ELSE(AGG_FOUND)
  SET(AGG_LIBRARIES)
  SET(AGG_INCLUDE_DIRS)
ENDIF(AGG_FOUND)

MARK_AS_ADVANCED(AGG_INCLUDE_DIR AGG_LIBRARY)

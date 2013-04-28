# - Find VIGRA
# Find VIGRA headers.
#
#  VIGRA_INCLUDE_DIRS - where to find VIGRA includes.
#  VIGRA_FOUND        - True if VIGRA found.

# Look for the header file.
FIND_PATH( VIGRA_INCLUDE_DIR NAMES vigra/accessor.hxx)

# handle the QUIETLY and REQUIRED arguments and set TBB_FOUND to TRUE if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VIGRA DEFAULT_MSG VIGRA_INCLUDE_DIR)

# Copy the results to the output variables.
IF(VIGRA_FOUND)
  SET(VIGRA_INCLUDE_DIRS ${VIGRA_INCLUDE_DIR})
ELSE(VIGRA_FOUND)
  SET(VIGRA_INCLUDE_DIRS)
ENDIF(VIGRA_FOUND)

MARK_AS_ADVANCED(VIGRA_INCLUDE_DIR)


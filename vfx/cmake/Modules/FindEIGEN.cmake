# - Find Eigen
# Find Eigen headers.
#
#  EIGEN_INCLUDE_DIRS - where to find EIGEN uncludes.
#  EIGEN_FOUND        - True if EIGEN found.

# Look for the header file.
FIND_PATH(EIGEN_INCLUDE_DIR NAMES Eigen/Eigen)

# handle the QUIETLY and REQUIRED arguments and set TBB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EIGEN DEFAULT_MSG EIGEN_INCLUDE_DIR)

# Copy the results to the output variables.
IF(EIGEN_FOUND)
  SET(EIGEN_INCLUDE_DIRS ${EIGEN_INCLUDE_DIR})
ELSE(EIGEN_FOUND)
  SET(EIGEN_INCLUDE_DIRS)
ENDIF(EIGEN_FOUND)

MARK_AS_ADVANCED(EIGEN_INCLUDE_DIR)


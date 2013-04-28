#==========
#
# Copyright (c) 2010, Dan Bethell.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of Dan Bethell nor the names of any
#       other contributors to this software may be used to endorse or
#       promote products derived from this software without specific prior
#       written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#==========
#
# Variables defined by this module:
#   ThreeDelight_FOUND    
#   ThreeDelight_INCLUDE_DIRS
#   ThreeDelight_COMPILE_FLAGS
#   ThreeDelight_LIBRARIES
#
# Usage: 
#   FIND_PACKAGE( ThreeDelight )
#   FIND_PACKAGE( ThreeDelight REQUIRED )
#
# Note:
# You can tell the module where ThreeDelight is installed by setting
# the ThreeDelight_INSTALL_PATH (or setting the DELIGHT environment
# variable before calling FIND_PACKAGE.
# 
# E.g. 
#   SET( ThreeDelight_INSTALL_PATH "/usr/local/ThreeDelight-9.0.0/Linux-x86_64" )
#   FIND_PACKAGE( ThreeDelight REQUIRED )
#
#==========

# our includes
FIND_PATH( ThreeDelight_INCLUDE_DIR ri.h $ENV{DELIGHT}/include ${ThreeDelight_INSTALL_PATH}/include)

# our library itself
FIND_LIBRARY( ThreeDelight_LIBRARY 3delight $ENV{DELIGHT}/lib ${ThreeDelight_INSTALL_PATH}/lib)

# did we find everything?
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "ThreeDelight" DEFAULT_MSG ThreeDelight_LIBRARY ThreeDelight_INCLUDE_DIR)

# Copy the results to the output variables.
IF(ThreeDelight_FOUND)
  SET(ThreeDelight_LIBRARIES ${ThreeDelight_LIBRARY})
  SET(ThreeDelight_INCLUDE_DIRS ${ThreeDelight_INCLUDE_DIR})
  SET( ThreeDelight_COMPILE_FLAGS "-DDELIGHT" )
ELSE(ThreeDelight_FOUND)
  SET(ThreeDelight_LIBRARIES)
  SET(ThreeDelight_INCLUDE_DIRS)
ENDIF(ThreeDelight_FOUND)

MARK_AS_ADVANCED(ThreeDelight_INCLUDE_DIR ThreeDelight_LIBRARY)

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
#   Nuke_FOUND    
#   Nuke_INCLUDE_DIR
#   Nuke_LIBRARIES
#   Nuke_LIBRARY_DIR
#
# Usage: 
#   FIND_PACKAGE( Nuke )
#   FIND_PACKAGE( Nuke REQUIRED )
#
# Note:
# You can tell the module where Nuke is installed by setting
# the Nuke_INSTALL_PATH (or setting the NDK_PATH environment
# variable before calling FIND_PACKAGE.
# 
# E.g. 
#   SET( Nuke_INSTALL_PATH "/usr/local/Nuke5.2v3" )
#   FIND_PACKAGE( Nuke REQUIRED )
#
#==========

# our includes
FIND_PATH( Nuke_INCLUDE_DIR DDImage/Op.h
  $ENV{NDK_PATH}/include
  ${Nuke_INSTALL_PATH}/include
  )

# our library
FIND_LIBRARY( Nuke_LIBRARIES DDImage
  $ENV{NDK_PATH}
  ${Nuke_INSTALL_PATH}
  )

# our library path
GET_FILENAME_COMPONENT( Nuke_LIBRARY_DIR ${Nuke_LIBRARIES} PATH )

# did we find everything?
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "Nuke" DEFAULT_MSG
  Nuke_INCLUDE_DIR
  Nuke_LIBRARIES
  Nuke_LIBRARY_DIR
  )

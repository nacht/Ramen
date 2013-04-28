CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 FATAL_ERROR)

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "CMake generation is not allowed within the source directory!")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake/Modules")

#PROJECT SETUP
PROJECT( BlDefocus)
SET( VERSION_MAJOR "0")
SET( VERSION_MINOR "0")
SET( VERSION_PATCH "1")
SET( VERSION_SUFFIX "")
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
SET( OFX_PLUGIN_NAME Bl_Defocus)

IF( WIN32)
    SET( CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../../include
                            ${PROJECT_SOURCE_DIR}/../../include/OpenEXR)

    SET( CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/../../lib)
ELSE( WIN32)
	SET( CMAKE_INCLUDE_PATH	/usr/local/include)
	SET( CMAKE_LIBRARY_PATH /usr/local/lib)
ENDIF( WIN32)

# compiler options
IF( NOT CMAKE_BUILD_TYPE)
    SET( CMAKE_BUILD_TYPE Release)
ENDIF( NOT CMAKE_BUILD_TYPE)

IF( CMAKE_BUILD_TYPE MATCHES Release)
    IF( CMAKE_COMPILER_IS_GNUCXX)
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -fexpensive-optimizations -mfpmath=sse")
    ENDIF( CMAKE_COMPILER_IS_GNUCXX)
ENDIF( CMAKE_BUILD_TYPE MATCHES Release)

# dependencies

# ofx things

SET( OFX_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../../ofx-1.2)

SET( OFX_SUPPORT_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/include)

SET( OFX_SUPPORT_SOURCES    ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsCore.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsImageEffect.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsInteract.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsLog.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsMultiThread.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsParams.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsProperty.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsPropertyValidation.cpp
                            ${PROJECT_SOURCE_DIR}/../../OfxSupport-1.1.3/Library/ofxsRect.cpp
                            )

# include paths
INCLUDE_DIRECTORIES(    ../
			${OFX_SUPPORT_INCLUDE_DIRS}
                        ${OFX_INCLUDE_DIRS}
			)

SET( PLUGIN_SOURCES 	register_plugins.cpp
			bl_defocus_plugin.cpp
			bl_zdefocus_plugin.cpp
                        bl_defocus_processor.cpp
                        CMP_defocus.c
                        BLI_rand.cpp
                        ImathRandom.cpp)

ADD_LIBRARY( ${OFX_PLUGIN_NAME} SHARED  ${PLUGIN_SOURCES}
                                	${OFX_SUPPORT_SOURCES})

SET_TARGET_PROPERTIES( ${OFX_PLUGIN_NAME} PROPERTIES SUFFIX ".ofx" PREFIX "")

IF( APPLE)
    SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${OFX_PLUGIN_NAME}.ofx.bundle/Contents/MacOS/)
ELSE( APPLE)

# TODO: use CMAKE_SIZEOF_VOID_P to detect 32 or 64 bits build and rename the dir as needed
# IF( CMAKE_SIZEOF_VOID_P EQUALS 4) -> 32 bits.
	IF( WIN32)
    		SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${OFX_PLUGIN_NAME}.ofx.bundle/Contents/Win64/)
	ELSE( WIN32)
    		SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${OFX_PLUGIN_NAME}.ofx.bundle/Contents/Linux-x86-64/)
	ENDIF( WIN32)
ENDIF( APPLE)


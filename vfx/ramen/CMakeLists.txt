CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 FATAL_ERROR)

#PROJECT SETUP
PROJECT( ramen)
SET( VERSION_MAJOR "0")
SET( VERSION_MINOR "6")
SET( VERSION_PATCH "2")
SET( VERSION_SUFFIX "")
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
SET( MAIN_DIR_NAME "ramen")

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "CMake generation for Ramen is not allowed within the source directory!")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

####################################################################
# edit the paths if needed

# add dirs common to all platforms
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/../cmake/Modules)

# platform specific paths
IF( WIN32)
    SET( CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../../include
                            ${PROJECT_SOURCE_DIR}/../../include/OpenEXR)

    SET( CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/../../lib)
ELSE( WIN32)
    IF( APPLE)
        # add paths where macports intalls headers and libs
        SET( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
        SET( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
    ELSE( APPLE)
        # linux and other unix systems
        SET( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
                                /usr/include/jpeg
				/usr/local/include/jpeg
				/usr/include/tiff
    				/usr/local/include/tiff)

        #SET( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH})
    ENDIF( APPLE)
ENDIF( WIN32)

####################################################################
# SSE support
OPTION( BUILD_SSE_SUPPORT "Build support for SSE" ON)

####################################################################
# compiler settings

# all compilers
IF( BUILD_SSE_SUPPORT)
    ADD_DEFINITIONS( -DRAMEN_SSE_SUPPORT)
ENDIF( BUILD_SSE_SUPPORT)

# visual C++
IF( MSVC)
    ADD_DEFINITIONS( -DNOMINMAX)
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_DEPRECATE)
    ADD_DEFINITIONS( -D_SCL_SECURE_NO_DEPRECATE)
    ADD_DEFINITIONS( -DBOOST_ALL_DYN_LINK)
    ADD_DEFINITIONS( -DOPENEXR_DLL)

    # disable some warnings
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4244 /wd4267 /wd4305")
ENDIF( MSVC)

# gcc
IF( CMAKE_COMPILER_IS_GNUCXX)
    IF( BUILD_SSE_SUPPORT)
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -mfpmath=sse")
    ENDIF( BUILD_SSE_SUPPORT)
ENDIF( CMAKE_COMPILER_IS_GNUCXX)

# looks like this does not work
IF( NOT CMAKE_BUILD_TYPE)
    SET( CMAKE_BUILD_TYPE Release)
ENDIF( NOT CMAKE_BUILD_TYPE)

# compiler settings for release builds
IF( CMAKE_BUILD_TYPE MATCHES Release)
    # gcc
    IF( CMAKE_COMPILER_IS_GNUCXX)
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -funroll-loops -fexpensive-optimizations")
    ENDIF( CMAKE_COMPILER_IS_GNUCXX)

    # visual C++
    IF( MSVC)
    ENDIF( MSVC)
ENDIF( CMAKE_BUILD_TYPE MATCHES Release)

####################################################################
# dependencies

# GL
FIND_PACKAGE( OpenGL REQUIRED)

# boost

# you can change this to "1.42.0" if needed.
# Don't use boost 1.40 or 1.41 as they don't work.
SET( Boost_ADDITIONAL_VERSIONS "1.43.0")

# change this to reflect your boost include path
IF( WIN32)
    # set to the default boost path on windows
    SET( BOOST_ROOT "C:/Boost")
    SET( BOOST_INCLUDEDIR "C:/Boost/include/boost-1_43")
    SET( BOOST_LIBRARYDIR "C:/Boost/lib")
ELSE( WIN32)
    SET( BOOST_INCLUDEDIR /usr/local/include/boost_1_43_0/)
    SET( BOOST_LIBRARYDIR /usr/local)
ENDIF( WIN32)

FIND_PACKAGE( Boost COMPONENTS filesystem thread signals system serialization python program_options 
				iostreams regex wave date_time REQUIRED)

# abort if boost 1.40 or 1.41 as they don't work with Ramen.
IF( Boost_VERSION STREQUAL "104000" OR Boost_VERSION STREQUAL "104100")
    MESSAGE( FATAL_ERROR "Boost 1.40 and 1.41 don't work, found ${Boost_VERSION}")
ENDIF( Boost_VERSION STREQUAL "104000" OR Boost_VERSION STREQUAL "104100")

MESSAGE( "Found Boost version ${Boost_VERSION}")

# base libs
FIND_PACKAGE( TBB REQUIRED)

# for now, we only need the headers
#FIND_PACKAGE( IECore REQUIRED)

# Qt
SET( QT_MT_REQUIRED false)
SET( QT_MIN_VERSION "4.0.0")
FIND_PACKAGE( Qt4 REQUIRED)
ADD_DEFINITIONS( -DQT_NO_KEYWORDS)

# OFX deps
SET( OFX_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../ofx-1.2)

# imageio libraries
FIND_PACKAGE( OpenEXR REQUIRED)
FIND_PACKAGE( JPEG REQUIRED)
FIND_PACKAGE( PNG REQUIRED)
FIND_PACKAGE( TIFF REQUIRED)
FIND_PACKAGE( OpenImageIO REQUIRED)

# more libs
FIND_PACKAGE( EIGEN REQUIRED)
FIND_PACKAGE( ZLIB REQUIRED)
FIND_PACKAGE( PythonLibs REQUIRED)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}
                     ${PROJECT_SOURCE_DIR}/../
                     ${PROJECT_SOURCE_DIR}/extern

                     ${QT_INCLUDE_DIR}
                     ${QT_QTCORE_INCLUDE_DIR}
                     ${QT_QTGUI_INCLUDE_DIR}
                     ${QT_QTOPENGL_INCLUDE_DIR}

		     ${Boost_INCLUDE_DIR}
		     ${TBB_INCLUDE_DIRS}

                     ${IECORE_INCLUDE_DIRS}

                     ${OFX_INCLUDE_DIRS}

		     ${OPENEXR_INCLUDE_DIR}
		     ${OPENEXR_INCLUDE_DIR}/..

		     ${JPEG_INCLUDE_DIR}
		     ${PNG_INCLUDE_DIR}
		     ${TIFF_INCLUDE_DIR}
                     ${OPENIMAGEIO_INCLUDE_DIRS}

		     ${EIGEN_INCLUDE_DIRS}
		     ${PYTHON_INCLUDE_PATH}
		   )

LINK_LIBRARIES( ${QT_QT_LIBRARIES}
                ${QT_QTCORE_LIBRARIES}
                ${QT_QTGUI_LIBRARIES}
                ${QT_QTOPENGL_LIBRARIES}

                ${OPENIMAGEIO_LIBRARIES}
		${OPENEXR_LIBRARIES}
		${JPEG_LIBRARIES}
		${PNG_LIBRARIES}
		${TIFF_LIBRARIES}

		${OPENGL_LIBRARIES}

		${Boost_LIBRARIES}
		${TBB_LIBRARIES}

                ${OFX_HOST_SUPPORT_LIBRARIES}

		${PYTHON_LIBRARIES}
		${ZLIB_LIBRARIES}
		)

ADD_SUBDIRECTORY( ramen)

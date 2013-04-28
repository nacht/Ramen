CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 FATAL_ERROR)

#PROJECT SETUP
PROJECT( rmaketiled)
SET( VERSION_MAJOR "0")
SET( VERSION_MINOR "6")
SET( VERSION_PATCH "0")
SET( VERSION_SUFFIX "")
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
SET( MAIN_DIR_NAME "rmaketiled")

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "CMake generation for Make Proxies is not allowed within the source directory!")
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
        SET( CMAKE_INCLUDE_PATH /usr/include/jpeg
				/usr/local/include/jpeg
				/usr/include/tiff
    				/usr/local/include/tiff)

        #SET( CMAKE_LIBRARY_PATH ????)
    ENDIF( APPLE)
ENDIF( WIN32)

####################################################################

# compiler settings

IF( MSVC)
    ADD_DEFINITIONS( -DNOMINMAX)
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_DEPRECATE)
    ADD_DEFINITIONS( -D_SCL_SECURE_NO_DEPRECATE)
    ADD_DEFINITIONS( -DBOOST_ALL_DYN_LINK)
    ADD_DEFINITIONS( -DOPENEXR_DLL)

    # disable some warnings
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4244 /wd4267 /wd4305")
ENDIF( MSVC)

IF( NOT CMAKE_BUILD_TYPE)
    SET( CMAKE_BUILD_TYPE Release)
ENDIF( NOT CMAKE_BUILD_TYPE)

# release
IF( CMAKE_BUILD_TYPE MATCHES Release)
    IF( CMAKE_COMPILER_IS_GNUCXX)
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -funroll-loops -fexpensive-optimizations")
    ENDIF( CMAKE_COMPILER_IS_GNUCXX)
ENDIF( CMAKE_BUILD_TYPE MATCHES Release)

####################################################################
# dependencies

# boost

# you can change this to "1.42.0" if needed.
# Don't use boost 1.40 or 1.41 as they don't work.
SET( Boost_ADDITIONAL_VERSIONS "1.43.0")

# change this to reflect your boost include path
IF( MSVC)
    SET( BOOST_ROOT "C:/Boost")
    SET( BOOST_INCLUDEDIR "C:/Boost/include/boost-1_43")
    SET( BOOST_LIBRARYDIR "C:/Boost/lib")
ELSE( MSVC)
    SET( BOOST_INCLUDEDIR /usr/local/include/boost_1_43_0/)
    SET( BOOST_LIBRARYDIR /usr/local)
ENDIF( MSVC)

FIND_PACKAGE( Boost COMPONENTS filesystem system thread signals REQUIRED)

# abort if boost 1.40 or 1.41 as they don't work with Make proxies.
IF( Boost_VERSION STREQUAL "104000" OR Boost_VERSION STREQUAL "104100")
    MESSAGE( FATAL_ERROR "Boost 1.40 and 1.41 don't work, found ${Boost_VERSION}")
ENDIF( Boost_VERSION STREQUAL "104000" OR Boost_VERSION STREQUAL "104100")

MESSAGE( "Found Boost version ${Boost_VERSION}")

# imageio libraries
FIND_PACKAGE( OpenEXR REQUIRED)
FIND_PACKAGE( JPEG REQUIRED)
FIND_PACKAGE( PNG REQUIRED)
FIND_PACKAGE( TIFF REQUIRED)
FIND_PACKAGE( OpenImageIO REQUIRED)
FIND_PACKAGE( ZLIB REQUIRED)

SET( RMAKE_TILED_SOURCES    main.cpp
                            Image.cpp
                            makeTiled.cpp)

SET( RAMEN_SOURCES          ../ramen/ramen/imageio/factory.cpp
                            ../ramen/ramen/imageio/format.cpp
                            ../ramen/ramen/imageio/reader.cpp
                            ../ramen/ramen/imageio/writer.cpp

                            ../ramen/ramen/imageio/cineon/cineon_format.cpp
                            ../ramen/ramen/imageio/cineon/cineon_reader.cpp
                            ../ramen/ramen/imageio/cineon/cineon_writer.cpp

                            ../ramen/ramen/imageio/dpx/dpxHeader.cpp
                            ../ramen/ramen/imageio/dpx/dpx_format.cpp
                            ../ramen/ramen/imageio/dpx/dpx_reader.cpp
                            ../ramen/ramen/imageio/dpx/dpx_writer.cpp

                            ../ramen/ramen/imageio/hdr/hdr_format.cpp
                            ../ramen/ramen/imageio/hdr/hdr_writer.cpp

                            ../ramen/ramen/imageio/jpg/jpg_format.cpp
                            ../ramen/ramen/imageio/jpg/jpg_reader.cpp
                            ../ramen/ramen/imageio/jpg/jpg_size.cpp
                            ../ramen/ramen/imageio/jpg/jpg_writer.cpp

                            ../ramen/ramen/imageio/oiio/oiio_reader.cpp
                            ../ramen/ramen/imageio/oiio/oiio_writer.cpp

                            ../ramen/ramen/imageio/png/png_format.cpp
                            ../ramen/ramen/imageio/png/png_writer.cpp

                            ../ramen/ramen/imageio/tga/tga_format.cpp
                            ../ramen/ramen/imageio/tga/tga_writer.cpp

                            ../ramen/ramen/imageio/tiff/tiff_format.cpp
                            ../ramen/ramen/imageio/tiff/tiff_writer.cpp

                            ../half/halfExpLogTable.cpp)

# loki
SET( LOKI_INCLUDES  ${PROJECT_SOURCE_DIR}/../Loki)

SET( LOKI_SOURCES   ${PROJECT_SOURCE_DIR}/../Loki/src/LevelMutex.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/OrderedStatic.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SafeFormat.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/Singleton.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SmallObj.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SmartPtr.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/StrongPtr.cpp)

# asl
ADD_DEFINITIONS( -DADOBE_STD_SERIALIZATION)

SET( ASL_INCLUDES   ${PROJECT_SOURCE_DIR}/../asl-1.0.42/)

SET( ASL_SOURCES    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/adam.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/adam_evaluate.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/adam_parser.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/any_regular.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/basic_sheet.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/empty.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/eve.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/eve_evaluate.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/eve_parser.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/expression_filter.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/expression_parser.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/extents.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/iomanip_asl_cel.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/iomanip.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/iomanip_javascript.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/iomanip_pdf.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/iomanip_xml.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/istream.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/lex_shared.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/lex_stream.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/localization.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/md5.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/memory.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/name.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/parser_shared.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/string.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/string_pool.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/test_configuration.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/thread_id.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/token.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/typeinfo.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/unicode.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/virtual_machine.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/xml_lex.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/xml_parser.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/xml_token.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/xstring.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/zuid.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/zuid_sys_dep.cpp
		    ${PROJECT_SOURCE_DIR}/../asl-1.0.42/source/zuid_uuid.cpp)

# setup includes, libs and executables

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}
                     ${PROJECT_SOURCE_DIR}/../
                     ${PROJECT_SOURCE_DIR}/../ramen/

		     ${Boost_INCLUDE_DIR}

		     ${OPENEXR_INCLUDE_DIR}
		     ${OPENEXR_INCLUDE_DIR}/..

		     ${JPEG_INCLUDE_DIR}
		     ${PNG_INCLUDE_DIR}
		     ${TIFF_INCLUDE_DIR}
                     ${OPENIMAGEIO_INCLUDE_DIRS}

                     ${LOKI_INCLUDES}
                     ${ASL_INCLUDES}
		   )

# linux needs the dl lib
IF( CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET( EXTRA_LIBRARIES dl)
ENDIF( CMAKE_SYSTEM_NAME MATCHES "Linux")

LINK_LIBRARIES( ${OPENIMAGEIO_LIBRARIES}
		${OPENEXR_LIBRARIES}
		${JPEG_LIBRARIES}
		${PNG_LIBRARIES}
		${TIFF_LIBRARIES}

		${Boost_LIBRARIES}

		${ZLIB_LIBRARIES}
                ${EXTRA_LIBRARIES}
		)

ADD_EXECUTABLE( rmaketiled ${RMAKE_TILED_SOURCES} ${RAMEN_SOURCES} ${LOKI_SOURCES} ${ASL_SOURCES})


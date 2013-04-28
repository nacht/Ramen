CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 FATAL_ERROR)

#PROJECT SETUP
PROJECT( asl)
SET( VERSION_MAJOR "1")
SET( VERSION_MINOR "0")
SET( VERSION_PATCH "42")
SET( VERSION_SUFFIX "")
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "CMake generation for Asl is not allowed within the source directory!")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

SET( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake/Modules)

# compiler options
IF( NOT CMAKE_BUILD_TYPE)
    SET( CMAKE_BUILD_TYPE Release)
ENDIF( NOT CMAKE_BUILD_TYPE)

IF( MSVC)
    ADD_DEFINITIONS( -DBOOST_ALL_DYN_LINK)
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_DEPRECATE)
    ADD_DEFINITIONS( -D_SCL_SECURE_NO_DEPRECATE)
ENDIF( MSVC)

ADD_DEFINITIONS( -DADOBE_STD_SERIALIZATION)

# edit if you have other version of boost
SET( Boost_ADDITIONAL_VERSIONS "1.43.0")

IF( MSVC)
    SET( CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../../include)
    SET( CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/../../lib)

    ADD_DEFINITIONS( -DNOMINMAX)
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275")

    SET( BOOST_ROOT "C:/Boost")
    SET( BOOST_INCLUDEDIR "C:/Boost/include/boost-1_43")
    SET( BOOST_LIBRARYDIR "C:/Boost/lib")
ELSE( MSVC)
    SET( BOOST_INCLUDEDIR /usr/local/include/boost_1_43_0/)
    SET( BOOST_LIBRARYDIR /usr/local)
ENDIF( MSVC)

FIND_PACKAGE( Boost COMPONENTS filesystem thread signals system date_time REQUIRED)
MESSAGE( "Found Boost version ${Boost_VERSION}")

FIND_PACKAGE( TBB REQUIRED)

MESSAGE( "Asl ${VERSION} will be built to install into ${CMAKE_INSTALL_PREFIX}")

SET( ASL_INCLUDES ./)
SET( ASL_SOURCES    source/adam.cpp
		    source/adam_evaluate.cpp
		    source/adam_parser.cpp
		    source/any_regular.cpp
		    source/basic_sheet.cpp
		    source/empty.cpp
		    source/eve.cpp
		    source/eve_evaluate.cpp
		    source/eve_parser.cpp
		    source/expression_filter.cpp
		    source/expression_parser.cpp
		    source/extents.cpp
		    source/iomanip_asl_cel.cpp
		    source/iomanip.cpp
		    source/iomanip_javascript.cpp
		    source/iomanip_pdf.cpp
		    source/iomanip_xml.cpp
		    source/istream.cpp
		    source/lex_shared.cpp
		    source/lex_stream.cpp
		    source/localization.cpp
		    source/md5.cpp
		    source/memory.cpp
		    source/name.cpp
		    source/parser_shared.cpp
		    source/string.cpp
		    source/string_pool.cpp
		    source/test_configuration.cpp
		    source/thread_id.cpp
		    source/token.cpp
		    source/typeinfo.cpp
		    source/unicode.cpp
		    source/virtual_machine.cpp
		    source/xml_lex.cpp
		    source/xml_parser.cpp
		    source/xml_token.cpp
		    source/xstring.cpp
		    source/zuid.cpp
		    source/zuid_sys_dep.cpp
		    source/zuid_uuid.cpp)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS} ${ASL_INCLUDES})

LINK_LIBRARIES( ${Boost_LIBRARIES} ${TBB_LIBRARIES})

ADD_LIBRARY( asl SHARED ${ASL_SOURCES})
SET_TARGET_PROPERTIES( asl PROPERTIES VERSION 1.0.42 SOVERSION 1)

# install
INSTALL( DIRECTORY adobe DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
INSTALL( TARGETS asl DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


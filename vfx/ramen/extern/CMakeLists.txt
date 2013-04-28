#loki
SET( LOKI_INCLUDES  ${PROJECT_SOURCE_DIR}/../Loki)

SET( LOKI_SOURCES   ${PROJECT_SOURCE_DIR}/../Loki/src/LevelMutex.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/OrderedStatic.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SafeFormat.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/Singleton.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SmallObj.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/SmartPtr.cpp
		    ${PROJECT_SOURCE_DIR}/../Loki/src/StrongPtr.cpp)

#agg
SET( AGG_INCLUDES ${PROJECT_SOURCE_DIR}/../agg-2.4
		  ${PROJECT_SOURCE_DIR}/../agg-2.4/agg)

SET( AGG_SOURCES    ../../agg-2.4/src/agg_arc.cpp
		    ../../agg-2.4/src/agg_arrowhead.cpp
		    ../../agg-2.4/src/agg_bezier_arc.cpp
		    ../../agg-2.4/src/agg_bspline.cpp
		    ../../agg-2.4/src/agg_curves.cpp
		    ../../agg-2.4/src/agg_embedded_raster_fonts.cpp
		    ../../agg-2.4/src/agg_gsv_text.cpp
		    ../../agg-2.4/src/agg_image_filters.cpp
		    ../../agg-2.4/src/agg_line_aa_basics.cpp
		    ../../agg-2.4/src/agg_line_profile_aa.cpp
		    ../../agg-2.4/src/agg_rounded_rect.cpp
		    ../../agg-2.4/src/agg_sqrt_tables.cpp
		    ../../agg-2.4/src/agg_trans_affine.cpp
		    ../../agg-2.4/src/agg_trans_double_path.cpp
		    ../../agg-2.4/src/agg_trans_single_path.cpp
		    ../../agg-2.4/src/agg_trans_warp_magnifier.cpp
		    ../../agg-2.4/src/agg_vcgen_bspline.cpp
		    ../../agg-2.4/src/agg_vcgen_contour.cpp
		    ../../agg-2.4/src/agg_vcgen_dash.cpp
		    ../../agg-2.4/src/agg_vcgen_markers_term.cpp
		    ../../agg-2.4/src/agg_vcgen_smooth_poly1.cpp
		    ../../agg-2.4/src/agg_vcgen_stroke.cpp
		    ../../agg-2.4/src/agg_vpgen_clip_polygon.cpp
		    ../../agg-2.4/src/agg_vpgen_clip_polyline.cpp
		    ../../agg-2.4/src/agg_vpgen_segmentator.cpp
		    #../../agg-2.4/src/agg_font_freetype.cpp
		    )
#noise lib
SET( NOISE_SOURCES  ../../noise/simplex_noise_generator.cpp)

#half
SET( HALF_SOURCES ../../half/halfExpLogTable.cpp)

#glew
ADD_DEFINITIONS( -DGLEW_STATIC)
INCLUDE( ${PROJECT_SOURCE_DIR}/extern/glew/CMakeLists.txt)

#OfxHostSupport
ADD_DEFINITIONS( -DHAVE_MEMMOVE)
ADD_DEFINITIONS( -DXML_STATIC)

IF( MSVC)
    ADD_DEFINITIONS( -DWINDOWS)
ENDIF( MSVC)

SET( OFX_HOST_SUPPORT_INCLUDES  ${PROJECT_SOURCE_DIR}/../OfxHostSupport/include/
                                ${PROJECT_SOURCE_DIR}/../OfxHostSupport/expat-2.0.1/lib)

SET( OFX_HOST_SUPPORT_SOURCES	${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhBinary.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhClip.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhHost.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhImageEffect.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhImageEffectAPI.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhInteract.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhMemory.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhParam.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhPluginAPICache.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhPluginCache.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhPropertySuite.cpp
				${PROJECT_SOURCE_DIR}/../OfxHostSupport/src/ofxhUtilities.cpp

				${PROJECT_SOURCE_DIR}/../OfxHostSupport/expat-2.0.1/lib/xmlparse.c
		    		${PROJECT_SOURCE_DIR}/../OfxHostSupport/expat-2.0.1/lib/xmlrole.c
		    		${PROJECT_SOURCE_DIR}/../OfxHostSupport/expat-2.0.1/lib/xmltok.c
		    		${PROJECT_SOURCE_DIR}/../OfxHostSupport/expat-2.0.1/lib/xmltok_ns.c)

#asl
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


###############################

SET( EXTERN_INCLUDES    ${LOKI_INCLUDES}
                        ${AGG_INCLUDES}
                        ${GLEW_INCLUDES}
                        ${OFX_HOST_SUPPORT_INCLUDES}
                        ${ASL_INCLUDES}
                        )

SET( EXTERN_SOURCES ${LOKI_SOURCES}
                    ${AGG_SOURCES}
		    ${GLEW_SOURCES}
                    ${NOISE_SOURCES}
                    ${HALF_SOURCES}
                    ${OFX_HOST_SUPPORT_SOURCES}
                    ${ASL_SOURCES}
		    )

# extern dir
INCLUDE( ${PROJECT_SOURCE_DIR}/extern/CMakeLists.txt)

SET( RAMEN_MOC_CLASSES	Qr/QrAbstractSpinBox.hpp
			Qr/QrCalculator.hpp
			Qr/QrColorButton.hpp
			Qr/QrContainerWidget.hpp
			Qr/QrDoubleSpinBox.hpp
			Qr/QrLabel.hpp
			Qr/QrLabelSeparator.hpp
			Qr/QrFormatWidget.hpp
			Qr/QrSmallToolButton.hpp
			Qr/QrSpinBox.hpp
			Qr/QrUtilityWindow.hpp

			Qr/ColorPicker/QrColorPicker.hpp
			Qr/ColorPicker/QrColorSwatch.hpp
			Qr/ColorPicker/QrExposurePicker.hpp
			Qr/ColorPicker/QrHuePicker.hpp
			Qr/ColorPicker/QrSaturationValuePicker.hpp

			app/QRamenApp.hpp

			flipbook/flipbook.hpp

			nodes/image_input/image_input_param.hpp

                        nodes/qshape/qshape_toolbar.hpp

			params/animated_param.hpp
			params/bool_param.hpp
			params/box2_param.hpp
			params/button_param.hpp
			params/color_param.hpp
			params/combo_group_param.hpp
			params/composite_param.hpp
			params/curve_param.hpp
			params/file_param.hpp
			params/float2_param.hpp
			params/float3_param.hpp
			params/float_param.hpp
			params/format_param.hpp
			params/group_param.hpp
			params/motion_blur_param.hpp
			params/numeric_param.hpp
			params/param.hpp
			params/popup_param.hpp
			params/proportional_param.hpp
			params/separator_param.hpp
			params/static_param.hpp
			params/tab_group_param.hpp
                        params/transform2_param.hpp

			ui/anim/anim_curves_view.hpp
			ui/anim/anim_editor.hpp
			ui/anim/anim_editor_toolbar.hpp
			ui/anim/track_model.hpp

			ui/composition_settings_dialog.hpp
			ui/composition_view.hpp
			ui/eyedropper_button.hpp
			ui/image_view.hpp
			ui/main_window.hpp
			ui/panel.hpp
			ui/preferences_dialog.hpp
			ui/render_dialog.hpp
			ui/render_flipbook_dialog.hpp
			ui/time_slider.hpp
			ui/user_interface.hpp
			ui/vcr_controls.hpp
			ui/viewer.hpp

			ui/python/python_console.hpp
			)

QT4_WRAP_CPP( RAMEN_MOC_SOURCES ${RAMEN_MOC_CLASSES})

SET( RAMEN_SOURCES  main.cpp

		    Qr/QrAbstractSpinBox.cpp
		    Qr/QrCalculator.cpp
		    Qr/QrColor.cpp
		    Qr/QrColorButton.cpp
		    Qr/QrContainerWidget.cpp
		    Qr/QrDoubleSpinBox.cpp
		    Qr/QrFormatWidget.cpp
		    Qr/QrLabel.cpp
		    Qr/QrLabelSeparator.cpp
		    Qr/QrSmallToolButton.cpp
		    Qr/QrSpinBox.cpp
		    Qr/QrUtilityWindow.cpp
		    Qr/parser.cpp

		    Qr/ColorPicker/QrColorPicker.cpp
		    Qr/ColorPicker/QrColorSwatch.cpp
		    Qr/ColorPicker/QrExposurePicker.cpp
		    Qr/ColorPicker/QrHuePicker.cpp
		    Qr/ColorPicker/QrSaturationValuePicker.cpp

		    anim/curve.cpp
		    anim/curve_algorithm.cpp
                    anim/curve_to_half_lut.cpp
		    anim/keyframe.cpp
		    anim/track.cpp
                    anim/point2.cpp

		    app/QRamenApp.cpp

		    app/application.cpp
		    app/composition.cpp
		    app/document.cpp
		    app/export_selected_nodes.cpp
		    app/import_composition.cpp
		    app/preferences.cpp
		    app/unique_name_map.cpp

		    flipbook/FileReadingThread.cpp
		    flipbook/flipbook.cpp
		    flipbook/ImageBuffers.cpp
		    flipbook/Timer.cpp

                    ggems/convex_opt.c
		    ggems/fitcurves.c
		    ggems/ggveclib.c
		    ggems/nearestpoint.c

		    image/box_blur.cpp
		    image/box_blur_channel.cpp
                    image/channel_range.cpp
		    image/color_bars.cpp
		    image/image_accumulator.cpp
		    image/image_buffer.cpp
		    image/image_cache.cpp
		    image/init_image_processing.cpp
		    image/percentile_filter.cpp

                    image/processing/generic/affine_warp.cpp
		    image/processing/generic/apply_color_matrix.cpp
		    image/processing/generic/apply_gamma.cpp
		    image/processing/generic/composite.cpp
		    image/processing/generic/convolve33.cpp
                    image/processing/generic/key_mix.cpp
		    image/processing/generic/layer_modes.cpp
		    image/processing/generic/math.cpp
		    image/processing/generic/projective_warp.cpp
                    image/processing/generic/resize.cpp
		    image/processing/generic/samplers.cpp
		    image/processing/generic/warp.cpp

		    image/processing.cpp

		    imageio/factory.cpp
		    imageio/format.cpp
		    imageio/multichannel_reader.cpp
		    imageio/reader.cpp
		    imageio/writer.cpp
		    imageio/cineon/cineon_format.cpp
		    imageio/cineon/cineon_reader.cpp
		    imageio/cineon/cineon_writer.cpp
		    imageio/dpx/dpxHeader.cpp
		    imageio/dpx/dpx_format.cpp
		    imageio/dpx/dpx_reader.cpp
		    imageio/dpx/dpx_writer.cpp
		    imageio/exr/exr_format.cpp
		    imageio/exr/exr_reader.cpp
		    imageio/exr/exr_writer.cpp
		    imageio/hdr/hdr_format.cpp
		    imageio/hdr/hdr_writer.cpp
		    imageio/imf_memory_stream.cpp
		    imageio/import_multichannel_exr.cpp
		    imageio/jpg/jpg_format.cpp
		    imageio/jpg/jpg_reader.cpp
		    imageio/jpg/jpg_writer.cpp
		    imageio/jpg/jpg_size.cpp
		    imageio/oiio/oiio_reader.cpp
		    imageio/oiio/oiio_writer.cpp
		    imageio/png/png_format.cpp
		    imageio/png/png_writer.cpp
		    imageio/tga/tga_format.cpp
		    imageio/tga/tga_writer.cpp
		    imageio/read_exr.cpp
		    imageio/write_exr.cpp
		    imageio/tiff/tiff_format.cpp
		    imageio/tiff/tiff_writer.cpp

                    ImathExt/ImathBoxAlgo.cpp
                    ImathExt/ImathMatrixAlgo.cpp

                    manipulators/draw_pick_controls.cpp
                    manipulators/manipulable.cpp
                    manipulators/manipulator.cpp
                    manipulators/point2_manipulator.cpp
                    manipulators/transform2_manipulator.cpp

		    memory/image_memory_manager.cpp

		    nodes/node.cpp
                    nodes/image_node.cpp
		    nodes/node_factory.cpp
                    nodes/reset_node_command.cpp

		    nodes/base/areaop_node.cpp
                    nodes/base/base_keyer_node.cpp
                    nodes/base/base_layer_node.cpp
		    nodes/base/distort_node.cpp
		    nodes/base/full_frame_node.cpp
		    nodes/base/generator_node.cpp
		    nodes/base/image_output_node.cpp
		    nodes/base/image_xform_node.cpp
		    nodes/base/multi_input_node.cpp
		    nodes/base/pointop_node.cpp
		    nodes/base/rescale_node.cpp
                    nodes/base/timeop_node.cpp

		    nodes/channel/copy_channels_node.cpp
		    nodes/channel/invert_node.cpp
		    nodes/channel/premult_node.cpp
		    nodes/channel/set_channels_node.cpp
		    nodes/channel/set_matte_node.cpp

                    nodes/color/adjust_hsv_node.cpp
		    nodes/color/channel_mixer_node.cpp
		    nodes/color/clamp_colors_node.cpp
		    nodes/color/color_correct_node.cpp
		    nodes/color/colorspace_node.cpp
		    nodes/color/curves_node.cpp
		    nodes/color/exposure_node.cpp
		    nodes/color/gamma_node.cpp
                    nodes/color/hsv_colorize_node.cpp
                    nodes/color/log_to_linear_node.cpp

                    nodes/distort/se_lens_distort_node.cpp

		    nodes/filter/blur_node.cpp
                    nodes/filter/blur_channels_node.cpp
                    nodes/filter/dv_chroma_blur_node.cpp
		    nodes/filter/percentile_filter_node.cpp
		    nodes/filter/unsharp_mask_node.cpp

		    nodes/image_input/image_input_info.cpp
		    nodes/image_input/image_input_node.cpp
		    nodes/image_input/image_input_param.cpp

		    nodes/input/color_bars_node.cpp
		    nodes/input/color_node.cpp
		    nodes/input/gradient_node.cpp
		    nodes/input/mandelbrot_node.cpp

		    nodes/key/color_diff_keyer_node.cpp
                    nodes/key/lum_key_node.cpp
		    nodes/key/spill_supressor_node.cpp

		    nodes/layer/add_mix_layer_node.cpp
		    nodes/layer/key_mix_layer_node.cpp
		    nodes/layer/layer_node.cpp
		    nodes/layer/multilayer_node.cpp

		    nodes/matte/alpha_levels_node.cpp
                    nodes/matte/alpha_ops_node.cpp

		    nodes/noise/add_grain_node.cpp
		    nodes/noise/add_noise_node.cpp

		    nodes/output/image_sequence_output_node.cpp

                    nodes/qshape/qshape_commands.cpp
                    nodes/qshape/qshape_convert_tool.cpp
                    nodes/qshape/qshape_create_tool.cpp
                    nodes/qshape/qshape_node.cpp
                    nodes/qshape/qshape_node_view.cpp
                    nodes/qshape/qshape_select_tool.cpp
                    nodes/qshape/qshape_toolbar.cpp
                    nodes/qshape/qshape_tool.cpp
                    nodes/qshape/shape.cpp
                    nodes/qshape/shape_renderer.cpp
                    nodes/qshape/triple.cpp

		    nodes/resize/resize_node.cpp

                    nodes/time/average_node.cpp

		    nodes/tonemap/global_tonemap_node.cpp

		    nodes/transform/corner_pin_node.cpp
		    nodes/transform/crop_node.cpp
                    nodes/transform/expand_node.cpp
		    nodes/transform/flip_node.cpp
		    nodes/transform/move2d_node.cpp
                    nodes/transform/transform_blur_node.cpp

		    nodes/util/select_node.cpp
		    nodes/util/set_dod_node.cpp

		    ofx/clip.cpp
		    ofx/host.cpp
		    ofx/image.cpp
		    ofx/image_effect.cpp
                    ofx/image_effect_plane.cpp
                    ofx/ofx_manipulator.cpp
		    ofx/ofx_node.cpp
		    ofx/param.cpp
		    ofx/plugin_manager.cpp
		    ofx/threading.cpp
		    ofx/util.cpp

		    ofx/params/boolean_param.cpp
		    ofx/params/choice_param.cpp
		    ofx/params/double2d_param.cpp
		    ofx/params/double_param.cpp
		    ofx/params/integer_param.cpp
		    ofx/params/pushbutton_param.cpp
		    ofx/params/rgb_param.cpp
		    ofx/params/rgba_param.cpp

		    params/animated_param.cpp
		    params/animated_param_command.cpp
		    params/bool_param.cpp
		    params/box2_param.cpp
		    params/button_param.cpp
		    params/color_param.cpp
		    params/combo_group_param.cpp
		    params/composite_param.cpp
		    params/curve_param.cpp
		    params/file_param.cpp
		    params/float2_param.cpp
		    params/float3_param.cpp
		    params/float_param.cpp
		    params/format_param.cpp
		    params/group_param.cpp
		    params/motion_blur_param.cpp
		    params/numeric_param.cpp
		    params/param.cpp
		    params/param_set.cpp
                    params/parameterised.cpp
		    params/popup_param.cpp
		    params/proportional_param.cpp
		    params/separator_param.cpp
		    params/static_param.cpp
		    params/static_param_command.cpp
		    params/tab_group_param.cpp
                    params/transform2_param.cpp

		    python/export_composition.cpp
		    python/export_document.cpp
		    python/export_node.cpp
		    python/export_param.cpp
		    python/export_ui.cpp
		    python/interpreter.cpp

		    render/render_context.cpp
		    render/render_flipbook.cpp
		    render/render_sequence.cpp
		    render/renderer.cpp

		    serialization/any_regular.cpp
		    serialization/create_archive.cpp

		    ui/add_node_command.cpp
		    ui/composition_settings_dialog.cpp
		    ui/composition_view.cpp
		    ui/composition_view_commands.cpp
		    ui/draw_composition.cpp
		    ui/edit_commands.cpp
		    ui/eyedropper_button.cpp
		    ui/image_view.cpp
		    ui/image_view_tool.cpp
		    ui/inspector.cpp
		    ui/main_window.cpp
		    ui/nice_numbers.cpp
		    ui/palette.cpp
		    ui/panel.cpp
		    ui/preferences_dialog.cpp
		    ui/render_dialog.cpp
		    ui/render_flipbook_dialog.cpp
		    ui/splash_screen.cpp
		    ui/time_slider.cpp
		    ui/user_interface.cpp
		    ui/vcr_controls.cpp
		    ui/viewer.cpp

		    ui/anim/anim_curves_view.cpp
		    ui/anim/anim_editor.cpp
		    ui/anim/anim_editor_command.cpp
		    ui/anim/anim_editor_select_tool.cpp
		    ui/anim/anim_editor_toolbar.cpp
		    ui/anim/anim_editor_tool.cpp
		    ui/anim/draw_pick_curves.cpp
		    ui/anim/track_model.cpp

		    ui/python/python_console.cpp

                    ui/viewport.cpp

		    undo/command.cpp
		    undo/undo_stack.cpp

		    util/format.cpp
		    util/md5_hash_generator.cpp
		    util/path.cpp
		    util/path_sequence.cpp
		    )

IF( BUILD_SSE_SUPPORT)
    SET( RAMEN_SIMD_SOURCES	image/processing/sse/affine_warp.cpp
                                image/processing/sse/composite.cpp
                                image/processing/sse/key_mix.cpp
				image/processing/sse/layer_modes.cpp
                                image/processing/sse/math.cpp
				image/processing/sse/projective_warp.cpp
                                image/processing/sse/resize.cpp
				image/processing/sse/samplers.cpp)
ENDIF( BUILD_SSE_SUPPORT)

#platform sources
IF( APPLE)
    SET( RAMEN_PLATFORM_SOURCES system/system_osx.cpp)
ENDIF( APPLE)

IF( WIN32)
    SET( RAMEN_PLATFORM_SOURCES system/system_win.cpp)
    SET( EXTRA_PLATFORM_LIBS Userenv)
ENDIF( WIN32)

IF( CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET( RAMEN_PLATFORM_SOURCES system/system_linux.cpp)
ENDIF( CMAKE_SYSTEM_NAME MATCHES "Linux")

IF( CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    SET( RAMEN_PLATFORM_SOURCES system/system_freebsd.cpp)
ENDIF( CMAKE_SYSTEM_NAME MATCHES "FreeBSD")

#resources
# this does not work, I don't know why. add resources manually
#QT4_ADD_RESOURCES( ${PROJECT_SOURCE_DIR}/resources/resources.qrc ${RAMEN_RESOURCES})
SET( RAMEN_RESOURCES ../resources/qrc_resources.cpp)

SET( RAMEN_ALL_SOURCES	${RAMEN_SOURCES}
			${RAMEN_MOC_SOURCES}
			${RAMEN_PLATFORM_SOURCES}
			${RAMEN_SIMD_SOURCES}
			${RAMEN_RESOURCES}
			${EXTERN_SOURCES}
			)

INCLUDE_DIRECTORIES( ${EXTERN_INCLUDES})
LINK_LIBRARIES( ${EXTRA_PLATFORM_LIBS})

IF( APPLE)
    ADD_EXECUTABLE( Ramen MACOSX_BUNDLE ${RAMEN_ALL_SOURCES}
			# add any extra resource here
		    )

    SET_TARGET_PROPERTIES( Ramen PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/osx/Info.plist)

ELSE( APPLE)
    ADD_EXECUTABLE( ramen ${RAMEN_ALL_SOURCES})
ENDIF( APPLE)

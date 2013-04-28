// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_PROCESSING_HPP
#define	RAMEN_IMAGE_PROCESSING_HPP

#include<boost/function.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

// basic math ops
extern boost::function<void ( const image_view_t&)> clear_image;
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> add_images;
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> sub_images;
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> mul_images;
extern boost::function<void ( const const_image_view_t&, float, const image_view_t&)> mul_image_scalar;
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> madd_images;
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> lerp_images;

extern boost::function<void ( const channel_view_t&)> clear_channel;
extern boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> add_channels;
extern boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> sub_channels;
extern boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> mul_channels;
extern boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, float, const channel_view_t&)> madd_channels;

extern boost::function<void ( const gray_image_view_t&)> clear_gray_image;
extern boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> add_gray_images;
extern boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> sub_gray_images;
extern boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> mul_gray_images;
extern boost::function<void ( const const_gray_image_view_t&, float, const gray_image_view_t&)> mul_gray_image_scalar;
extern boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> madd_gray_images;
extern boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> lerp_gray_images;

// alpha
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> premultiply;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> unpremultiply;

// invert
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgba;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgb;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_alpha;
extern boost::function<void ( const const_channel_view_t&, const channel_view_t&)> invert_channel;

// color space conversions
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_yuv;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_hsv;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_lab;

extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_yuv_to_rgb;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_hsv_to_rgb;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_lab_to_rgb;

// color
extern boost::function<void ( const const_image_view_t&, const image_view_t&, float)> apply_gamma;
extern boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M44f&)> apply_color_matrix;

// statistics
extern boost::function<image::pixel_t ( const const_image_view_t&)> mean;

// transform
extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> affine_warp_nearest;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> affine_warp_bilinear;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> affine_warp_bicubic;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> projective_warp_nearest;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> projective_warp_bilinear;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> projective_warp_bicubic;

// transform + non-black border modes
extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> affine_warp_bilinear_tile;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&, const Imath::M33d&)> affine_warp_bilinear_mirror;

// resize
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_nearest;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bilinear;
extern boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bicubic;

// layer
extern boost::function<void ( const const_image_view_t&, const const_image_view_t&, 
				const image_view_t&, float)> composite_over;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_add;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mul;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_sub;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mix;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_screen;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_overlay;

extern boost::function<void ( const const_image_view_t&, const image_view_t&,
                                                float)> composite_zero_overlay;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_diff;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_max;

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_min;

// warps

typedef boost::function<Imath::V2f ( const Imath::V2f&)> warp_function_type;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_type& )> warp_bilinear;

extern boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_type& )> warp_bicubic;

// key mix

extern boost::function<void ( const const_image_view_t&, const const_image_view_t&,
                                const const_channel_view_t&, const image_view_t&)> key_mix;

// convolve
extern boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M33f&)> convolve33;

} // namespace
} // namespace

#endif	/* _PROCESSING_HPP */

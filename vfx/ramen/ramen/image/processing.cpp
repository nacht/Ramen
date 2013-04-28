// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing.hpp>

#include<ramen/image/processing/generic/math.hpp>
#include<ramen/image/processing/generic/stats.hpp>

#include<ramen/image/processing/generic/premultiply.hpp>
#include<ramen/image/processing/generic/invert.hpp>

#include<ramen/image/processing/generic/rgb_to_yuv.hpp>
#include<ramen/image/processing/generic/rgb_to_hsv.hpp>
#include<ramen/image/processing/generic/rgb_to_lab.hpp>

#include<ramen/image/processing/generic/apply_gamma.hpp>
#include<ramen/image/processing/generic/apply_color_matrix.hpp>

#include<ramen/image/processing/generic/composite.hpp>

#include<ramen/image/processing/generic/affine_warp.hpp>
#include<ramen/image/processing/generic/projective_warp.hpp>
#include<ramen/image/processing/generic/resize.hpp>
#include<ramen/image/processing/generic/warp.hpp>

#include<ramen/image/processing/generic/key_mix.hpp>

#include<ramen/image/processing/generic/convolve33.hpp>

namespace ramen
{
namespace image
{

// basic math
boost::function<void ( const image_view_t&)> clear_image = generic::clear_image;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> add_images = generic::add_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> sub_images = generic::sub_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> mul_images = generic::mul_images;
boost::function<void ( const const_image_view_t&, float, const image_view_t&)> mul_image_scalar = generic::mul_image_scalar;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> madd_images = generic::madd_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> lerp_images = generic::lerp_images;

boost::function<void ( const channel_view_t&)> clear_channel= generic::clear_channel;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> add_channels = generic::add_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> sub_channels = generic::sub_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> mul_channels = generic::mul_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, float, const channel_view_t&)> madd_channels = generic::madd_channels;

boost::function<void ( const gray_image_view_t&)> clear_gray_image;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> add_gray_images = generic::add_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> sub_gray_images = generic::sub_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> mul_gray_images = generic::mul_gray_images;
boost::function<void ( const const_gray_image_view_t&, float, const gray_image_view_t&)> mul_gray_image_scalar = generic::mul_gray_image_scalar;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> madd_gray_images = generic::madd_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> lerp_gray_images = generic::lerp_gray_images;

// channels
boost::function<void ( const const_image_view_t&, const image_view_t&)> premultiply = generic::premultiply;
boost::function<void ( const const_image_view_t&, const image_view_t&)> unpremultiply = generic::unpremultiply;

boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgba = generic::invert_rgba;
boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgb = generic::invert_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_alpha = generic::invert_alpha;
boost::function<void ( const const_channel_view_t&, const channel_view_t&)> invert_channel = generic::invert_channel;

// color space
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_yuv = generic::convert_rgb_to_yuv;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_hsv = generic::convert_rgb_to_hsv;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_lab = generic::convert_rgb_to_lab;

boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_yuv_to_rgb = generic::convert_yuv_to_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_hsv_to_rgb = generic::convert_hsv_to_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_lab_to_rgb = generic::convert_lab_to_rgb;

// color
boost::function<void ( const const_image_view_t&, const image_view_t&, float)> apply_gamma = generic::apply_gamma;
boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M44f&)> apply_color_matrix = generic::apply_color_matrix;

// stats
boost::function<image::pixel_t ( const const_image_view_t&)> mean = generic::mean;

// transform
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> affine_warp_nearest = generic::affine_warp_nearest;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> affine_warp_bilinear = generic::affine_warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> affine_warp_bicubic = generic::affine_warp_bicubic;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> projective_warp_nearest = generic::projective_warp_nearest;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> projective_warp_bilinear = generic::projective_warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> projective_warp_bicubic = generic::projective_warp_bicubic;

// transform + non black border modes
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> affine_warp_bilinear_tile = generic::affine_warp_bilinear_tile;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const Imath::M33d&,
		    const Imath::M33d&)> affine_warp_bilinear_mirror = generic::affine_warp_bilinear_mirror;

// resize

boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_nearest  = generic::resize_nearest;
boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bilinear = generic::resize_bilinear;
boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bicubic  = generic::resize_bicubic;

// composite
boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_over = generic::composite_over;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_add = generic::composite_add;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mul = generic::composite_mul;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_sub = generic::composite_sub;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mix = generic::composite_mix;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_screen = generic::composite_screen;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_overlay = generic::composite_overlay;

boost::function<void ( const const_image_view_t&, const image_view_t&,
                                                float)> composite_zero_overlay = generic::composite_zero_overlay;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_diff = generic::composite_diff;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_max = generic::composite_max;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_min = generic::composite_min;

// warps
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_type& )> warp_bilinear = generic::warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_type& )> warp_bicubic = generic::warp_bicubic;

// key mix

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
                       const const_channel_view_t&, const image_view_t&)> key_mix = generic::key_mix;

// convolve
boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M33f&)> convolve33 = generic::convolve33;

} // namespace
} // namespace

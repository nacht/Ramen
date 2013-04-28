// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/affine_warp.hpp>

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void affine_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    affine_warp<point_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

void affine_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    affine_warp<bilinear_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

void affine_warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    affine_warp<bicubic_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

// non-black
void affine_warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    affine_warp_non_black_border_mode<bilinear_tile_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

void affine_warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    affine_warp_non_black_border_mode<bilinear_mirror_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

} // namespace
} // namespace
} // namespace

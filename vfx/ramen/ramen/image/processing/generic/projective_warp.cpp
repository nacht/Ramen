// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/projective_warp.hpp>

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void projective_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    projective_warp<point_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

void projective_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    projective_warp<bilinear_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

void projective_warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    projective_warp<bicubic_sampler_t>( src_area, src, dst_area, dst, xform, inv_xform);
}

} // namespace
} // namespace
} // namespace

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_AFFINE_WARP_HPP
#define RAMEN_IMAGE_GENERIC_AFFINE_WARP_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

template<class Sampler>
class affine_warp_fn
{
public:

    affine_warp_fn( const Imath::Box2i& src_area, const const_image_view_t& src,
		      const Imath::Box2i& dst_area, const image_view_t& dst,
		      const Imath::Box2i& active_area, const Imath::M33d& inv_xform) : src_area_(src_area), src_(src),
											      dst_area_(dst_area), dst_( dst),
											      active_area_(active_area),
											      inv_xform_( inv_xform),
											      s_( src_area, src)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
	for( int j = r.begin(); j < r.end(); ++j)
	{
            Imath::V2d p ( Imath::V2d( active_area_.min.x, j) * inv_xform_);
            Imath::V2d p1( Imath::V2d( active_area_.max.x, j) * inv_xform_);
            Imath::V2d inc( ( p1 - p) / ( active_area_.max.x - active_area_.min.x));

	    for( int i = active_area_.min.x; i <= active_area_.max.x; ++i)
	    {
		dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( p);
                p += inc;
	    }
	}
    }

private:

    Sampler s_;
    const Imath::Box2i& src_area_, dst_area_, active_area_;
    const const_image_view_t& src_;
    const image_view_t& dst_;
    const Imath::M33d& inv_xform_;
};

} // detail

template<class Sampler>
void affine_warp( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    Imath::Box2i active_area( transform( src_area, xform));
    active_area = intersect( active_area, dst_area);

    tbb::parallel_for( tbb::blocked_range<int>( active_area.min.y, active_area.max.y+1),
		      detail::affine_warp_fn<Sampler>( src_area, src, dst_area, dst, active_area, inv_xform),
		      tbb::auto_partitioner());
}

template<class Sampler>
void affine_warp_non_black_border_mode( const Imath::Box2i& src_area, const const_image_view_t& src,
                                        const Imath::Box2i& dst_area, const image_view_t& dst,
                                        const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    Imath::Box2i active_area( dst_area);

    tbb::parallel_for( tbb::blocked_range<int>( active_area.min.y, active_area.max.y+1),
		      detail::affine_warp_fn<Sampler>( src_area, src, dst_area, dst, active_area, inv_xform),
		      tbb::auto_partitioner());
}

void affine_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

void affine_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

void affine_warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

// non black border modes
void affine_warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

void affine_warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

} // namespace
} // namespace
} // namespace

#endif

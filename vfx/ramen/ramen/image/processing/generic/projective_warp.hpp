// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_PROJECTIVE_WARP_HPP
#define RAMEN_IMAGE_GENERIC_PROJECTIVE_WARP_HPP

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
class projective_warp_fn
{
public:

    projective_warp_fn( const Imath::Box2i& src_area, const const_image_view_t& src,
		      const Imath::Box2i& dst_area, const image_view_t& dst, const Imath::M33d& inv_xform) : src_area_(src_area), src_(src),
                                                                                                            dst_area_(dst_area), dst_( dst),
                                                                                                            inv_xform_( inv_xform),
                                                                                                            s_( src_area, src)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
	for( int j = r.begin(); j < r.end(); ++j)
	{
            Imath::V3d p ( Imath::V3d( dst_area_.min.x, j, 1) * inv_xform_);
            Imath::V3d p1( Imath::V3d( dst_area_.max.x, j, 1) * inv_xform_);
            Imath::V3d inc( ( p1 - p) / ( dst_area_.max.x - dst_area_.min.x));

	    for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
	    {
                if( p.z != 0)
                    dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( Imath::V2d( p.x / p.z, p.y / p.z));

                p += inc;
	    }
	}
    }

private:

    Sampler s_;
    const Imath::Box2i& src_area_, dst_area_;
    const const_image_view_t& src_;
    const image_view_t& dst_;
    const Imath::M33d& inv_xform_;
};

} // detail

template<class Sampler>
void projective_warp( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform)
{
    tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y + 1),
		      detail::projective_warp_fn<Sampler>( src_area, src, dst_area, dst, inv_xform),
		      tbb::auto_partitioner());
}

void projective_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

void projective_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

void projective_warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const Imath::M33d& xform, const Imath::M33d& inv_xform);

} // namespace
} // namespace
} // namespace

#endif

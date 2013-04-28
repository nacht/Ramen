// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#include<ramen/image/processing/generic/warp.hpp>

#include<cmath>

#include<boost/function.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace
{
template<class Sampler>
class warp_fn
{
public:

    warp_fn( const Imath::Box2i& src_area, const const_image_view_t& src,
	    const Imath::Box2i& dst_area, const image_view_t& dst,
	    const boost::function<Imath::V2f ( const Imath::V2f&)>& fun) : dst_( dst), fun_( fun), s_( src_area, src)
    {
	dst_area_ = dst_area;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
	for( int j = r.begin(); j < r.end(); ++j)
	{
	    for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
	    {
		Imath::V2f p( i, j);
		Imath::V2f q( fun_( p));
		dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( q);
	    }
	}
    }

private:

    Sampler s_;
    Imath::Box2i dst_area_;
    const image_view_t& dst_;
    boost::function<Imath::V2f ( const Imath::V2f&)> fun_;
};

template<class Sampler>
void warp( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		    const boost::function<Imath::V2f ( const Imath::V2f&)>& wfun)
{
    tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1),
		      warp_fn<Sampler>( src_area, src, dst_area, dst, wfun),
		      tbb::auto_partitioner());
}

} // unnamed

void warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const boost::function<Imath::V2f ( const Imath::V2f&)>& wfun)
{
    warp<bilinear_sampler_t>( src_area, src, dst_area, dst, wfun);
}

void warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const boost::function<Imath::V2f ( const Imath::V2f&)>& wfun)
{
    warp<bicubic_sampler_t>( src_area, src, dst_area, dst, wfun);
}

} // namespace
} // namespace
} // namespace

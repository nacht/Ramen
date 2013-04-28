// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/box_blur.hpp>

#include<algorithm>

#include<adobe/algorithm/clamp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/image/color.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{

namespace
{

inline float thereshold( float x, float lo)
{
    if( x < lo)
        return 0.0f;

    return x;
}

struct box_blur_rgba_fn
{
    box_blur_rgba_fn( const const_image_view_t& src, const image_view_t& dst, float radius) : src_(src), dst_(dst), radius_(radius) {}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
	int radius = (int) radius_;
	float norm = 1.0f / ( 2 * radius + 1);

	for( std::size_t y = r.begin(); y != r.end(); ++y)
	{
	    const_image_view_t::x_iterator src_it( src_.row_begin( y));
	    image_view_t::y_iterator dst_it( dst_.col_begin( y));

	    pixel_t accum( 0, 0, 0, 0);

            for ( int i = -radius; i <= radius; i++ )
	    {
		int indx = adobe::clamp( i, 0, (int) src_.width() - 1);
		get_color( accum, red_t())   += get_color( src_it[indx], red_t());
		get_color( accum, green_t()) += get_color( src_it[indx], green_t());
		get_color( accum, blue_t())  += get_color( src_it[indx], blue_t());
		get_color( accum, alpha_t()) += get_color( src_it[indx], alpha_t());
            }

	    for( int x = 0; x < src_.width(); ++x)
	    {
		get_color( dst_it[x], red_t())   = thereshold( get_color( accum, red_t())  * norm, 0.00001f);
		get_color( dst_it[x], green_t()) = thereshold( get_color( accum, green_t())* norm, 0.00001f);
		get_color( dst_it[x], blue_t())  = thereshold( get_color( accum, blue_t()) * norm, 0.00001f);
		get_color( dst_it[x], alpha_t()) = thereshold( get_color( accum, alpha_t())* norm, 0.00001f);

                int i1 = std::min( x + radius + 1, (int) src_.width()-1);
                int i2 = std::max( x - radius, 0);

		get_color( accum, red_t())   += get_color( src_it[i1], red_t())	    - get_color( src_it[i2], red_t());
		get_color( accum, green_t()) += get_color( src_it[i1], green_t())   - get_color( src_it[i2], green_t());
		get_color( accum, blue_t())  += get_color( src_it[i1], blue_t())    - get_color( src_it[i2], blue_t());
		get_color( accum, alpha_t()) += get_color( src_it[i1], alpha_t())   - get_color( src_it[i2], alpha_t());
	    }
	}
    }

    const const_image_view_t& src_;
    const image_view_t& dst_;
    float radius_;
};


struct box_blur_rgba_fract_fn
{
    box_blur_rgba_fract_fn( const const_image_view_t& src, const image_view_t& dst, float radius) : src_(src), dst_(dst), radius_(radius)
    {
    }

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
	float radius = radius_ - std::floor( (double) radius_);
	float norm = 1.0f / ( 1.0f + 2.0f * radius);

	for( std::size_t y = r.begin(); y != r.end(); ++y)
	{
	    const_image_view_t::x_iterator src_it( src_.row_begin( y));
	    image_view_t::y_iterator dst_it( dst_.col_begin( y));

	    *dst_it = *src_it;
	    pixel_t accum;

	    for( int x = 1; x < src_.width()-1; ++x)
	    {
		get_color( accum, red_t())   = (get_color( src_it[x-1], red_t())    + get_color( src_it[x+1], red_t())) * radius;
		get_color( accum, green_t()) = (get_color( src_it[x-1], green_t())  + get_color( src_it[x+1], green_t())) * radius;
		get_color( accum, blue_t())  = (get_color( src_it[x-1], blue_t())   + get_color( src_it[x+1], blue_t())) * radius;
		get_color( accum, alpha_t()) = (get_color( src_it[x-1], alpha_t())  + get_color( src_it[x+1], alpha_t())) * radius;

		get_color( accum, red_t())   += get_color( src_it[x], red_t());
		get_color( accum, green_t()) += get_color( src_it[x], green_t());
		get_color( accum, blue_t())  += get_color( src_it[x], blue_t());
		get_color( accum, alpha_t()) += get_color( src_it[x], alpha_t());

		get_color( accum, red_t())   = thereshold( get_color( accum, red_t())   * norm, 0.00001f);
		get_color( accum, green_t()) = thereshold( get_color( accum, green_t()) * norm, 0.00001f);
		get_color( accum, blue_t())  = thereshold( get_color( accum, blue_t())  * norm, 0.00001f);
		get_color( accum, alpha_t()) = thereshold( get_color( accum, alpha_t()) * norm, 0.00001f);

		dst_it[x] = accum;
	    }
	}
    }

    const const_image_view_t& src_;
    const image_view_t& dst_;
    float radius_;
};

} // unnamed

void box_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
	boost::gil::copy_pixels( src, dst);
	return;
    }

    boost::gil::rgba32f_image_t buffer( src.height(), src.width());

    // run first iter
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			box_blur_rgba_fn( src, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
			box_blur_rgba_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
		       box_blur_rgba_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		     box_blur_rgba_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    for( int i = 1; i < iters; ++i)
    {
	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                    box_blur_rgba_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
	                    box_blur_rgba_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                   box_blur_rgba_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		         box_blur_rgba_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());
    }
}

} // namespace
} // namespace

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/box_blur.hpp>

#include<algorithm>

#include<adobe/algorithm/clamp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

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

template<class ConstGrayView, class GrayView>
struct box_blur_gray_fn
{
    box_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, float radius) : src_( src), dst_( dst), radius_( radius) {}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
	int radius = (int) radius_;
	float norm = 1.0f / ( 2.0f * radius + 1.0f);

	for( std::size_t y = r.begin(); y != r.end(); ++y)
	{
	    typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
	    typename GrayView::y_iterator dst_it( dst_.col_begin( y));

            float accum = 0.0f;

            for ( int i = -radius; i <= radius; i++ )
	    {
		int indx = adobe::clamp( i, 0, (int) src_.width() - 1);
		accum += src_it[indx][0];
            }

	    for( int x = 0; x < src_.width(); ++x)
	    {
		dst_it[x][0] = thereshold( accum * norm, 0.00001f);

                int i1 = std::min( x + radius + 1, (int) src_.width()-1);
                int i2 = std::max( x - radius, 0);

		accum += src_it[i1][0] - src_it[i2][0];
	    }
	}
    }

    const ConstGrayView& src_;
    const GrayView& dst_;
    float radius_;
};

template<class ConstGrayView, class GrayView>
struct box_blur_gray_fract_fn
{
    box_blur_gray_fract_fn( const ConstGrayView& src, const GrayView& dst, float radius) : src_( src), dst_( dst), radius_( radius)
    {
    }

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
	float radius = radius_ - std::floor( (double) radius_);
	float norm = 1.0f / ( 2.0f * radius + 1.0f);

	for( std::size_t y = r.begin(); y != r.end(); ++y)
	{
	    typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
	    typename GrayView::y_iterator dst_it( dst_.col_begin( y));

	    float accum;

	    *dst_it = *src_it;

	    for( int x = 1; x < src_.width()-1; ++x)
	    {
		accum  = ( src_it[x-1][0] + src_it[x+1][0]) * radius;
		accum += src_it[x][0];
		accum *= norm;
		dst_it[x] = thereshold( accum, 0.00001f);
	    }
	}
    }

    const ConstGrayView& src_;
    const GrayView& dst_;
    float radius_;
};

template<class ConstGrayView, class GrayView>
box_blur_gray_fn<ConstGrayView, GrayView> make_box_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, float radius)
{
    return box_blur_gray_fn<ConstGrayView, GrayView>( src, dst, radius);
}

template<class ConstGrayView, class GrayView>
box_blur_gray_fract_fn<ConstGrayView, GrayView> make_box_blur_gray_fract_fn( const ConstGrayView& src, const GrayView& dst, float radius)
{
    return box_blur_gray_fract_fn<ConstGrayView, GrayView>( src, dst, radius);
}

} // unnamed

void box_blur_channel( const const_channel_view_t& src, const channel_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
	boost::gil::copy_pixels( src, dst);
	return;
    }

    boost::gil::gray32f_image_t buffer( src.height(), src.width());

    // run first iter
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			make_box_blur_gray_fn( src, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
			make_box_blur_gray_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
		       make_box_blur_gray_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		     make_box_blur_gray_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    for( int i = 1; i < iters; ++i)
    {
	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                    make_box_blur_gray_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
	                    make_box_blur_gray_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                   make_box_blur_gray_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		         make_box_blur_gray_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());
    }
}

void box_blur( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
	boost::gil::copy_pixels( src, dst);
	return;
    }

    boost::gil::gray32f_image_t buffer( src.height(), src.width());

    // run first iter
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			make_box_blur_gray_fn( src, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
			make_box_blur_gray_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
		       make_box_blur_gray_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		     make_box_blur_gray_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

    for( int i = 1; i < iters; ++i)
    {
	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                    make_box_blur_gray_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
	                    make_box_blur_gray_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
	                   make_box_blur_gray_fract_fn( dst, view( buffer), hradius), tbb::auto_partitioner());

	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, buffer.height()),
		         make_box_blur_gray_fract_fn( const_view( buffer), dst, vradius), tbb::auto_partitioner());
    }
}

} // namespace
} // namespace

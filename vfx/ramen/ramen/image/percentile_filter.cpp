// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/percentile_filter.hpp>

#include<vector>
#include<cmath>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

namespace
{

template<class GraySrcView, class GrayDstView>
void percentile_filter( const tbb::blocked_range<std::size_t>& range, const GraySrcView& src, const GrayDstView& dst, int radius, float percentile)
{
    const int buckets = 1 << 16;
    std::vector<int> histogram( buckets);

    // make the filter edge profile
    std::vector<int> edge( radius * 2 + 1);

    for (int i = 0; i < 2 * radius + 1; i++)
        edge[i] = (int)( std::sqrt( (double) radius * radius - ( i - radius) * ( i-radius)) + 0.0001);

    for( int y = range.begin(); y < range.end(); y++)
    {
	// initialize the histogram for this scanline
	for( int i = 0; i < buckets; i++)
	    histogram[i] = 0;

	int lteq = 0;
	int total = 0;
	int medianBucket = buckets / 2;

	for (int i = 0; i < 2*radius+1; i++)
	{
	    int xoff = edge[i];
	    int yoff = i - radius;

	    if( y + yoff >= src.height())
		break;

	    if( y + yoff < 0)
		continue;

	    for (int j = 0; j <= xoff; j++)
	    {
		if (j >= src.width())
		    break;

		float val = src( j, y + yoff)[0];
		int bucket = (( half) val).bits();
		histogram[bucket]++;

		if (bucket <= medianBucket)
		    lteq++;

		total++;
	    }
	}

	for( int x = 0; x < src.width(); x++)
	{
	    // adjust the median bucket downwards
	    while( lteq > total * percentile && medianBucket > 0)
	    {
		lteq -= histogram[medianBucket];
		medianBucket--;
	    }

	    // adjust the median bucket upwards
	    while (lteq <= total * percentile && medianBucket < buckets-1)
	    {
		medianBucket++;
		lteq += histogram[medianBucket];
	    }

	    half result;
	    result.setBits( medianBucket);
	    dst( x, y)[0] = result;

	    // move the histogram to the right
	    for (int i = 0; i < radius*2+1; i++)
	    {
		int xoff = edge[i];
		int yoff = i - radius;

		if (y + yoff >= src.height())
		    break;

		if (y + yoff < 0)
		    continue;

		// subtract old value
		if (x - xoff >= 0)
		{
		    float val = src( x-xoff, y+yoff)[0];
		    int bucket = (( half) val).bits();
		    histogram[bucket]--;

		    if (bucket <= medianBucket)
			lteq--;

		    total--;
		}

		// add new value
		if (x + xoff + 1 < src.width())
		{
		    float val = src( x+xoff+1, y+yoff)[0];
		    int bucket = (( half) val).bits();
		    histogram[bucket]++;

		    if (bucket <= medianBucket)
			lteq++;

		    total++;
		}
	    }
	}
    }
}

template<class GraySrcView, class GrayDstView>
struct apply_percentile_filter
{
public:

    apply_percentile_filter( const GraySrcView& src, const GrayDstView& dst, int radius, float percentile) : src_( src), dst_( dst)
    {
	radius_ = radius;
	percentile_ = percentile;
    }

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
	percentile_filter( r, src_, dst_, radius_, percentile_);
    }

private:

    const GraySrcView& src_;
    const GrayDstView& dst_;
    int radius_;
    float percentile_;
};

}

void percentile_filter_channel( const const_channel_view_t& src, const channel_view_t& dst, int radius, float percent)
{
    assert( radius > 0);
    assert( percent >= 0);
    assert( percent <= 1);

    assert( src.width() == dst.width());
    assert( src.height() == dst.height());

    float percentile = percent;

    if( percentile == 1.0f)
	percentile = 0.999f;

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
                        apply_percentile_filter<const_channel_view_t, channel_view_t>( src, dst, radius, percentile),
                        tbb::auto_partitioner());
}

void percentile_filter_rgba( const const_image_view_t& src, const image_view_t& dst, int radius, float percent)
{
    percentile_filter_channel( boost::gil::nth_channel_view( src, 0), boost::gil::nth_channel_view( dst, 0), radius, percent);
    percentile_filter_channel( boost::gil::nth_channel_view( src, 1), boost::gil::nth_channel_view( dst, 1), radius, percent);
    percentile_filter_channel( boost::gil::nth_channel_view( src, 2), boost::gil::nth_channel_view( dst, 2), radius, percent);
    percentile_filter_channel( boost::gil::nth_channel_view( src, 3), boost::gil::nth_channel_view( dst, 3), radius, percent);
}

void percentile_filter_rgb( const const_image_view_t& src, const image_view_t& dst, int radius, float percent)
{
    percentile_filter_channel( boost::gil::nth_channel_view( src, 0), boost::gil::nth_channel_view( dst, 0), radius, percent);
    percentile_filter_channel( boost::gil::nth_channel_view( src, 1), boost::gil::nth_channel_view( dst, 1), radius, percent);
    percentile_filter_channel( boost::gil::nth_channel_view( src, 2), boost::gil::nth_channel_view( dst, 2), radius, percent);
    boost::gil::copy_pixels( boost::gil::nth_channel_view( src, 3), boost::gil::nth_channel_view( dst, 3));
}

void percentile_filter_alpha( const const_image_view_t& src, const image_view_t& dst, int radius, float percent)
{
    boost::gil::copy_pixels( boost::gil::nth_channel_view( src, 0), boost::gil::nth_channel_view( dst, 0));
    boost::gil::copy_pixels( boost::gil::nth_channel_view( src, 1), boost::gil::nth_channel_view( dst, 1));
    boost::gil::copy_pixels( boost::gil::nth_channel_view( src, 2), boost::gil::nth_channel_view( dst, 2));
    percentile_filter_channel( boost::gil::nth_channel_view( src, 3), boost::gil::nth_channel_view( dst, 3), radius, percent);
}

void dilate( const const_gray_image_view_t& src, const gray_image_view_t& dst, float radius)
{
    float percent = radius > 0 ? 0.999f : 0;
    float rad = radius > 0 ? radius : -radius;
    int r = std::floor( rad);
    float fract = rad - r;

    // nop
    if( r == 0 && fract == 0)
    {
        boost::gil::copy_pixels( src, dst);
        return;
    }

    // only fractional case
    if( r == 0 && fract != 0)
    {
        tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
                        apply_percentile_filter<const_gray_image_view_t, gray_image_view_t>( src, dst, 1, percent),
                        tbb::auto_partitioner());

        lerp_gray_images( dst, src, fract, dst);
        return;
    }

    // normal case
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
                        apply_percentile_filter<const_gray_image_view_t, gray_image_view_t>( src, dst, r, percent),
                        tbb::auto_partitioner());

    if( fract != 0)
    {
        gray_image_t tmp( src.width(), src.height());

        tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
                        apply_percentile_filter<const_gray_image_view_t, gray_image_view_t>( dst, boost::gil::view( tmp), 1, percent),
                        tbb::auto_partitioner());

        lerp_gray_images( boost::gil::view( tmp), dst, fract, dst);
    }
}

} // image
} // ramen

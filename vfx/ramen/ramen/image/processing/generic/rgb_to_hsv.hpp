// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RGBTOHSV_HPP
#define	RAMEN_IMAGE_GENERIC_RGBTOHSV_HPP

#include<algorithm>
#include<cmath>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct rgb_to_hsv
{
    pixel_t operator()( const pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t());
	float g = boost::gil::get_color( src, boost::gil::green_t());
	float b = boost::gil::get_color( src, boost::gil::blue_t());

	// normalize RGB to 0 .. 1
	if( r < 0) r = 0;
	if( g < 0) g = 0;
	if( b < 0) b = 0;

	float scale = std::max( r, std::max( g, b));

	if( scale < 1.0f)
	    scale = 1.0f;

	r /= scale;
	g /= scale;
	b /= scale;

	float maxv = std::max( r, std::max( g, b));
	float minv = std::min( r, std::min( g, b));

	float h = 0, s = 0, v = maxv * scale;

	if( maxv != minv)
	{
	    const float f = ( r == minv) ? ( g - b) : (( g == minv) ? ( b - r) : ( r - g)),
			i = ( r == minv) ? 3.0f : (( g == minv) ? 5.0f : 1.0f);

	    h = ( i - f / ( maxv - minv));

	    if( h >= 6.0f)
		h -= 6.0f;

	    h /= 6.0f;
	    s = ( maxv - minv) / maxv;
	}

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t())   = h;
	boost::gil::get_color( result, boost::gil::green_t()) = s;
	boost::gil::get_color( result, boost::gil::blue_t())  = v;
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

struct hsv_to_rgb
{
    pixel_t operator()( const pixel_t& src) const
    {
	float h = boost::gil::get_color( src, boost::gil::red_t());
	float s = boost::gil::get_color( src, boost::gil::green_t());
	float v = boost::gil::get_color( src, boost::gil::blue_t());

	float scale = std::max( v, 1.0f);
	v /= scale;

	float r = 0, g = 0, b = 0;

	if( h == 0 && s == 0)
	    r = g = b = v;
	else
	{
	    h *= 6.0f;
	    const int i = (int) std::floor( h);
	    const float f = ( i & 1) ? ( h - i) : ( 1.0f - h + i),
			m = v * ( 1.0f - s),
			n = v * ( 1.0f - s * f);
	    switch(i)
	    {
	    case 6:
	    case 0: r = v; g = n; b = m; break;

	    case 1: r = n; g = v; b = m; break;
	    case 2: r = m; g = v; b = n; break;
	    case 3: r = m; g = n; b = v; break;
	    case 4: r = n; g = m; b = v; break;
	    case 5: r = v; g = m; b = n; break;
	    }
	}

	r *= scale;
	g *= scale;
	b *= scale;

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t())   = r;
	boost::gil::get_color( result, boost::gil::green_t()) = g;
	boost::gil::get_color( result, boost::gil::blue_t())  = b;
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

} // namespace

void convert_rgb_to_hsv( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::rgb_to_hsv());
}

void convert_hsv_to_rgb( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::hsv_to_rgb());
}

} // namespace
} // namespace
} // namespace

#endif


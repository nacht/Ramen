// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_PREMULTIPLY_HPP
#define	RAMEN_IMAGE_GENERIC_PREMULTIPLY_HPP

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
struct alpha_premult_fun
{
    pixel_t operator()( const pixel_t& src) const
    {
	pixel_t r;
	float a = boost::gil::get_color( src, boost::gil::alpha_t());
	boost::gil::get_color( r, boost::gil::red_t())	= boost::gil::get_color( src, boost::gil::red_t()) * a;
	boost::gil::get_color( r, boost::gil::green_t())= boost::gil::get_color( src, boost::gil::green_t()) * a;
	boost::gil::get_color( r, boost::gil::blue_t())	= boost::gil::get_color( src, boost::gil::blue_t()) * a;
	boost::gil::get_color( r, boost::gil::alpha_t())= a;
	return r;
    }
};

struct alpha_unpremult_fun
{
    pixel_t operator()( const pixel_t& src) const
    {
	pixel_t r;
	float a = boost::gil::get_color( src, boost::gil::alpha_t());

	if( a != 0)
	{
	    float ia = 1.0f / a;
	    boost::gil::get_color( r, boost::gil::red_t())  = boost::gil::get_color( src, boost::gil::red_t()) * ia;
	    boost::gil::get_color( r, boost::gil::green_t())= boost::gil::get_color( src, boost::gil::green_t()) * ia;
	    boost::gil::get_color( r, boost::gil::blue_t()) = boost::gil::get_color( src, boost::gil::blue_t()) * ia;
	    boost::gil::get_color( r, boost::gil::alpha_t())= a;
	}
	else
	{
	    boost::gil::get_color( r, boost::gil::red_t())  = 0;
	    boost::gil::get_color( r, boost::gil::green_t())= 0;
	    boost::gil::get_color( r, boost::gil::blue_t()) = 0;
	    boost::gil::get_color( r, boost::gil::alpha_t())= 0;
	}

	return r;
    }
};

} // namespace

void premultiply( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::alpha_premult_fun());
}

void unpremultiply( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::alpha_unpremult_fun());
}

} // namespace
} // namespace
} // namespace

#endif	/* _PREMULTIPLY_HPP */

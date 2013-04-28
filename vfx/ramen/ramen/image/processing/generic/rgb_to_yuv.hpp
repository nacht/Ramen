// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RGBTOYUV_HPP
#define	RAMEN_IMAGE_GENERIC_RGBTOYUV_HPP

#include<ramen/image/typedefs.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct rgb_to_yuv
{
    pixel_t operator()( const pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t());
	float g = boost::gil::get_color( src, boost::gil::green_t());
	float b = boost::gil::get_color( src, boost::gil::blue_t());
	float y = ( 0.299f * r) + ( 0.587f * g) + ( 0.114f * b);

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t()) = y;
	boost::gil::get_color( result, boost::gil::green_t()) = 0.492f * ( b - y);
	boost::gil::get_color( result, boost::gil::blue_t())  = 0.877f * ( r - y);
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

struct yuv_to_rgb
{
    pixel_t operator()( const pixel_t& src) const
    {
	float y = boost::gil::get_color( src, boost::gil::red_t());
	float u = boost::gil::get_color( src, boost::gil::green_t());
	float v = boost::gil::get_color( src, boost::gil::blue_t());

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t()) = y + 1.140f * v;
	boost::gil::get_color( result, boost::gil::green_t()) = y - 0.395f * u - 0.581f * v;
	boost::gil::get_color( result, boost::gil::blue_t())  = y + 2.032f * u;
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

} // namespace

void convert_rgb_to_yuv( const const_image_view_t& src, const image_view_t& dst)
{
    boost::gil::tbb_transform_pixels( src, dst, detail::rgb_to_yuv());
}

void convert_yuv_to_rgb( const const_image_view_t& src, const image_view_t& dst)
{
    boost::gil::tbb_transform_pixels( src, dst, detail::yuv_to_rgb());
}

} // namespace
} // namespace
} // namespace

#endif	/* _PREMULTIPLY_HPP */

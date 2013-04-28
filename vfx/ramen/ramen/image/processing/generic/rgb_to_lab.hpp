// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RGBTOLAB_HPP
#define	RAMEN_IMAGE_GENERIC_RGBTOLAB_HPP

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

void rgb2lab( float linear_r, float linear_g, float linear_b, float& ll, float& la, float& lb)
{
    float x = 0.412453f*linear_r + 0.357580f*linear_g + 0.180423f*linear_b;
    float y = 0.212671f*linear_r + 0.715160f*linear_g + 0.072169f*linear_b;
    float z = 0.019334f*linear_r + 0.119193f*linear_g + 0.950227f*linear_b;

    float xg = std::pow( x / 0.950456f, 0.3333333f);
    float yg = std::pow( y, 0.3333333f);
    float zg = std::pow( z / 1.088754f, 0.3333333f);

    ll = y < 0.008856f ? 903.3f * y : 116.0f * yg - 16.0f;
    la = 500.0f*(xg - yg);
    lb = 200.0f*(yg - zg);
}

void lab2rgb( float l, float a, float b, float& rr, float& rg, float& rb, float gamma = 1.0f)
{
    float y  = l < 8.0f ? l / 903.3f : std::pow((l + 16.0f) / 116.0f, 3.0f);
    float yg = std::pow( y, 1.0f / 3.0f);
    float x  = std::pow( a / 500.0f + yg, 3.0f) * 0.950456f;
    float z  = std::pow(-b / 200.0f + yg, 3.0f) * 1.088754f;

    rr =  3.2404813432f*x - 1.5371515163f*y - 0.4985363262f*z;
    rg = -0.9692549500f*x + 1.8759900015f*y + 0.0415559266f*z;
    rb =  0.0556466391f*x - 0.2040413384f*y + 1.0573110696f*z;
}

struct rgb_to_lab
{
    pixel_t operator()( const pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t());
	float g = boost::gil::get_color( src, boost::gil::green_t());
	float b = boost::gil::get_color( src, boost::gil::blue_t());

	float ll, la, lb;
	rgb2lab( r, g, b, ll, la, lb);

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t())   = ll;
	boost::gil::get_color( result, boost::gil::green_t()) = la;
	boost::gil::get_color( result, boost::gil::blue_t())  = lb;
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

struct lab_to_rgb
{
    pixel_t operator()( const pixel_t& src) const
    {
	float ll = boost::gil::get_color( src, boost::gil::red_t());
	float la = boost::gil::get_color( src, boost::gil::green_t());
	float lb = boost::gil::get_color( src, boost::gil::blue_t());

	float rr, rg, rb;
	lab2rgb( ll, la, lb, rr, rg, rb);

	pixel_t result;
	boost::gil::get_color( result, boost::gil::red_t())   = rr;
	boost::gil::get_color( result, boost::gil::green_t()) = rg;
	boost::gil::get_color( result, boost::gil::blue_t())  = rb;
	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }
};

} // namespace

void convert_rgb_to_lab( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::rgb_to_lab());
}

void convert_lab_to_rgb( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::lab_to_rgb());
}

} // namespace
} // namespace
} // namespace

#endif	/* _PREMULTIPLY_HPP */

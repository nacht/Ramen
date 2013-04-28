//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_LABCOLORSPACE_HPP
#define GIL_EXTENSION_LABCOLORSPACE_HPP

#include<cmath>

#include<boost/gil/gil_all.hpp>
#include<boost/type_traits/is_floating_point.hpp>

/*
	This code only works for float images.
	TODO: add some static asserts or even better, make 
	it more generic so that it works also with non float channels.
*/

namespace boost
{ 
namespace gil
{

struct luminance_t {};
struct a_color_t {};
struct b_color_t {};

typedef mpl::vector3<luminance_t, a_color_t, b_color_t> lab_t;
typedef layout<lab_t> lab_layout_t;

template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,lab_t> >::view_t
planar_lab_view(int width, int height, IC l, IC a, IC b, std::ptrdiff_t rowsize_in_bytes)
{
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,lab_t> >::view_t RView;
    return RView(width, height, typename RView::locator(planar_pixel_iterator<IC,lab_t>(l,a,b),rowsize_in_bytes));
}

// typedefs
GIL_DEFINE_ALL_TYPEDEFS(32f,lab)

// color conversion
namespace detail
{

void rgb2lab( float r, float g, float b, float& ll, float& la, float& lb, float gamma = 1.0f)
{
	float linear_r, linear_g, linear_b;
	
	if( gamma != 1.0f)
	{
		linear_r = std::pow( r, gamma);
		linear_g = std::pow( g, gamma);
		linear_b = std::pow( b, gamma);
	}
	else
	{
		linear_r = r;
		linear_g = g;
		linear_b = b;
	}

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
	
	if( gamma != 1.0f)
	{
		rr = std::pow( rr, gamma);
		rg = std::pow( rg, gamma);
		rb = std::pow( rb, gamma);
	}
}

const float minl = 0.0f    , maxl = 100.0f;
const float mina = 86.1813f, maxa = 98.2351f;
const float minb = 107.862f, maxb = 94.4758f;

} // namespace detail

struct rgb2lab_color_converter
{
    rgb2lab_color_converter( float gamma = 1.0f) : gamma_(gamma) {}

    template <typename P1, typename P2>
    void operator()( const P1& src, P2& dst) const
    {
	bits32f temp_red   = channel_convert<bits32f>( get_color( src, red_t()   ));
	bits32f temp_green = channel_convert<bits32f>( get_color( src, green_t() ));
	bits32f temp_blue  = channel_convert<bits32f>( get_color( src, blue_t()  ));

	float l, a, b;
	detail::rgb2lab( (float) temp_red, (float) temp_green, (float) temp_blue, l, a, b, gamma_);

	get_color( dst, luminance_t()) = channel_convert<typename color_element_type<luminance_t, P2>::type>((bits32f) l);
	get_color( dst, a_color_t())   = channel_convert<typename color_element_type<a_color_t  , P2>::type>((bits32f) a);
	get_color( dst, b_color_t())   = channel_convert<typename color_element_type<b_color_t  , P2>::type>((bits32f) b);
    }

private:
   
   float gamma_;
};

struct lab2rgb_color_converter
{
    lab2rgb_color_converter( float gamma = 1.0f) : gamma_( gamma) {}
	
    template <typename P1, typename P2>
    void operator()( const P1& src, P2& dst) const
    {
	bits32f temp_l = channel_convert<bits32f>( get_color( src, luminance_t()));
	bits32f temp_a = channel_convert<bits32f>( get_color( src, a_color_t()));
	bits32f temp_b = channel_convert<bits32f>( get_color( src, b_color_t()));

	float r, g, b;
	detail::lab2rgb( (float) temp_l, (float) temp_a, (float) temp_b, r, g, b);

	get_color( dst, red_t())   = channel_convert<typename color_element_type<red_t  , P2>::type>((bits32f) r);
	get_color( dst, green_t()) = channel_convert<typename color_element_type<green_t, P2>::type>((bits32f) g);
	get_color( dst, blue_t())  = channel_convert<typename color_element_type<blue_t , P2>::type>((bits32f) b);
    }

private:

    float gamma_;
};

template<>
struct default_color_converter_impl<rgb_t,lab_t>
{
    template <typename P1, typename P2>
    inline void operator()( const P1& src, P2& dst) const { cc_( src, dst);}

private:

    rgb2lab_color_converter cc_;
};

template<>
struct default_color_converter_impl<lab_t,rgb_t>
{
    template <typename P1, typename P2>
    inline void operator()( const P1& src, P2& dst) const { cc_( src, dst);}

private:

    lab2rgb_color_converter cc_;
};

// handle rgba as well

template<>
struct default_color_converter_impl<rgba_t,lab_t>
{
    template <typename P1, typename P2>
    inline void operator()( const P1& src, P2& dst) const { cc_( src, dst);}

private:

    rgb2lab_color_converter cc_;
};

template<>
struct default_color_converter_impl<lab_t,rgba_t>
{
    template <typename P1, typename P2>
    inline void operator()( const P1& src, P2& dst) const
    { 
       cc_( src, dst);
	//boost::gil::get_color( dst, boost::gil::alpha_t()) = 1;
    }

private:

    lab2rgb_color_converter cc_;
};

}
}  // namespace boost::gil

#endif


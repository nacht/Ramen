// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/math.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct add_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
	return pixel_t( boost::gil::get_color( a, boost::gil::red_t())   + boost::gil::get_color( b, boost::gil::red_t()),
			boost::gil::get_color( a, boost::gil::green_t()) + boost::gil::get_color( b, boost::gil::green_t()),
			boost::gil::get_color( a, boost::gil::blue_t())  + boost::gil::get_color( b, boost::gil::blue_t()),
			boost::gil::get_color( a, boost::gil::alpha_t()) + boost::gil::get_color( b, boost::gil::alpha_t()));
    }

    channel_pixel_t operator()( const channel_pixel_t& a, const channel_pixel_t& b) const
    {
	return channel_pixel_t( a[0] + b[0]);
    }
};

struct sub_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
	return pixel_t( boost::gil::get_color( a, boost::gil::red_t())   - boost::gil::get_color( b, boost::gil::red_t()),
			boost::gil::get_color( a, boost::gil::green_t()) - boost::gil::get_color( b, boost::gil::green_t()),
			boost::gil::get_color( a, boost::gil::blue_t())  - boost::gil::get_color( b, boost::gil::blue_t()),
			boost::gil::get_color( a, boost::gil::alpha_t()) - boost::gil::get_color( b, boost::gil::alpha_t()));
    }

    channel_pixel_t operator()( const channel_pixel_t& a, const channel_pixel_t& b) const
    {
	return channel_pixel_t( a[0] - b[0]);
    }
};

struct mul_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
	return pixel_t( boost::gil::get_color( a, boost::gil::red_t())   * boost::gil::get_color( b, boost::gil::red_t()),
			boost::gil::get_color( a, boost::gil::green_t()) * boost::gil::get_color( b, boost::gil::green_t()),
			boost::gil::get_color( a, boost::gil::blue_t())  * boost::gil::get_color( b, boost::gil::blue_t()),
			boost::gil::get_color( a, boost::gil::alpha_t()) * boost::gil::get_color( b, boost::gil::alpha_t()));
    }

    channel_pixel_t operator()( const channel_pixel_t& a, const channel_pixel_t& b) const
    {
	return channel_pixel_t( a[0] * b[0]);
    }
};

struct madd_pixels
{
    madd_pixels( float factor) : f_( factor) {}

    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
	return pixel_t( boost::gil::get_color( a, boost::gil::red_t())   + f_ * boost::gil::get_color( b, boost::gil::red_t()),
			boost::gil::get_color( a, boost::gil::green_t()) + f_ * boost::gil::get_color( b, boost::gil::green_t()),
			boost::gil::get_color( a, boost::gil::blue_t())  + f_ * boost::gil::get_color( b, boost::gil::blue_t()),
			boost::gil::get_color( a, boost::gil::alpha_t()) + f_ * boost::gil::get_color( b, boost::gil::alpha_t()));
    }

    channel_pixel_t operator()( const channel_pixel_t& a, const channel_pixel_t& b) const
    {
	return channel_pixel_t( a[0] + f_ * b[0]);
    }

    float f_;
};

struct mul_pixel_scalar
{
    mul_pixel_scalar( float f) : f_(f) {}

    pixel_t operator()( const pixel_t& p) const
    {
	return pixel_t( boost::gil::get_color( p, boost::gil::red_t())   * f_,
			boost::gil::get_color( p, boost::gil::green_t()) * f_,
			boost::gil::get_color( p, boost::gil::blue_t())  * f_,
			boost::gil::get_color( p, boost::gil::alpha_t()) * f_);
    }

    channel_pixel_t operator()( const channel_pixel_t& p) const
    {
	return channel_pixel_t( p[0] * f_);
    }

    float f_;
};

struct lerp_pixels
{
    lerp_pixels( float t) : t_( t), inv_t_( 1.0f - t)
    {
    }

    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
	return pixel_t( (boost::gil::get_color( a, boost::gil::red_t())   * t_) +
			    (boost::gil::get_color( b, boost::gil::red_t())   * inv_t_),

			(boost::gil::get_color( a, boost::gil::green_t())   * t_) +
			    (boost::gil::get_color( b, boost::gil::green_t())   * inv_t_),

			(boost::gil::get_color( a, boost::gil::blue_t())   * t_) +
			    (boost::gil::get_color( b, boost::gil::blue_t())   * inv_t_),

			(boost::gil::get_color( a, boost::gil::alpha_t())   * t_) +
			    (boost::gil::get_color( b, boost::gil::alpha_t())   * inv_t_));
    }

    channel_pixel_t operator()( const channel_pixel_t& a, const channel_pixel_t& b) const
    {
	return channel_pixel_t( (a[0] * t_) + ( b[0] * inv_t_));
    }

    float t_, inv_t_;
};

} // namespace

// images
void clear_image( const image_view_t& v)     { boost::gil::fill_pixels( v, image::pixel_t( 0, 0, 0, 0));}

void add_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::add_pixels());
}

void sub_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::sub_pixels());
}

void mul_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::mul_pixels());
}

void mul_image_scalar( const const_image_view_t& a, float f, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, result, detail::mul_pixel_scalar( f));
}

void madd_images( const const_image_view_t& a, const const_image_view_t& b, float f, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::madd_pixels( f));
}

void lerp_images( const const_image_view_t& a, const const_image_view_t& b, float t, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::lerp_pixels( t));
}

// gray images
void clear_gray_image( const gray_image_view_t& v) { boost::gil::fill_pixels( v, image::gray_pixel_t( 0));}

void add_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::add_pixels());
}

void sub_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::sub_pixels());
}

void mul_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::mul_pixels());
}

void mul_gray_image_scalar( const const_gray_image_view_t& a, float f, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, result, detail::mul_pixel_scalar( f));
}

void madd_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, float f, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::madd_pixels( f));
}

void lerp_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, float t, const gray_image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::lerp_pixels( t));
}

// channels
void clear_channel( const channel_view_t& v) { boost::gil::fill_pixels( v, image::channel_pixel_t( 0));}

void add_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::add_pixels());
}

void sub_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::sub_pixels());
}

void mul_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::mul_pixels());
}

void madd_channels( const const_channel_view_t& a, const const_channel_view_t& b, float f, const channel_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::madd_pixels( f));
}

} // namespace
} // namespace
} // namespace

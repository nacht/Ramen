// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_INVERT_HPP
#define	RAMEN_IMAGE_GENERIC_INVERT_HPP

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

struct invert_rgba_fun
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
	typename PixelRef::value_type r;
	boost::gil::get_color( r, boost::gil::red_t())	= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::red_t()));
	boost::gil::get_color( r, boost::gil::green_t())= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::green_t()));
	boost::gil::get_color( r, boost::gil::blue_t())	= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::blue_t()));
	boost::gil::get_color( r, boost::gil::alpha_t())= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::alpha_t()));
	return r;
    }
};

struct invert_rgb_fun
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
	typename PixelRef::value_type r;
	boost::gil::get_color( r, boost::gil::red_t())	= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::red_t()));
	boost::gil::get_color( r, boost::gil::green_t())= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::green_t()));
	boost::gil::get_color( r, boost::gil::blue_t())	= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::blue_t()));
	boost::gil::get_color( r, boost::gil::alpha_t())= boost::gil::get_color( p, boost::gil::alpha_t());
	return r;
    }
};

struct invert_alpha_fun
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
	typename PixelRef::value_type r;
	boost::gil::get_color( r, boost::gil::red_t())	= boost::gil::get_color( p, boost::gil::red_t());
	boost::gil::get_color( r, boost::gil::green_t())= boost::gil::get_color( p, boost::gil::green_t());
	boost::gil::get_color( r, boost::gil::blue_t())	= boost::gil::get_color( p, boost::gil::blue_t());
	boost::gil::get_color( r, boost::gil::alpha_t())= boost::gil::channel_invert( boost::gil::get_color( p, boost::gil::alpha_t()));
	return r;
    }
};

struct invert_channel_fun
{
    template<class PixelRef>
    typename PixelRef::value_type operator()( const PixelRef& p)
    {
	typename PixelRef::value_type r;
	r[0] = boost::gil::channel_invert( p[0]);
	return r;
    }
};

} // namespace

void invert_rgba( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::invert_rgba_fun());
}

void invert_rgb( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::invert_rgb_fun());
}

void invert_alpha( const const_image_view_t& src, const image_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::invert_alpha_fun());
}

void invert_channel( const const_channel_view_t&src, const channel_view_t& dst)
{
    assert( src.width() == dst.width());
    assert( src.height() == dst.height());
    boost::gil::tbb_transform_pixels( src, dst, detail::invert_channel_fun());
}

} // namespace
} // namespace
} // namespace

#endif	/* _INVERT_HPP */

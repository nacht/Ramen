// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_STATS_HPP
#define	RAMEN_IMAGE_GENERIC_STATS_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct pixel_accum_fun
{
    pixel_accum_fun() : accum( 0, 0, 0, 0) {}

    void operator()( const pixel_t& p)
    {
	boost::gil::get_color( accum, boost::gil::red_t())   += boost::gil::get_color( p, boost::gil::red_t());
	boost::gil::get_color( accum, boost::gil::green_t()) += boost::gil::get_color( p, boost::gil::green_t());
	boost::gil::get_color( accum, boost::gil::blue_t())  += boost::gil::get_color( p, boost::gil::blue_t());
	boost::gil::get_color( accum, boost::gil::alpha_t()) += boost::gil::get_color( p, boost::gil::alpha_t());
    }

    pixel_t accum;
};

} // detail

pixel_t mean( const const_image_view_t& v)
{
    detail::pixel_accum_fun f;
    boost::gil::for_each_pixel( v, f);
    std::size_t num_pixels = v.width() * v.height();

    return pixel_t( f.accum[0] / num_pixels,
                    f.accum[1] / num_pixels,
                    f.accum[2] / num_pixels,
                    f.accum[3] / num_pixels);
}

} // namespace
} // namespace
} // namespace

#endif	/* _MATH_HPP */

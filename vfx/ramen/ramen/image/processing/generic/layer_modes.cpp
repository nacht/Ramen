// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/layer_modes.hpp>

#include<adobe/algorithm/clamp.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

inline image::pixel_t lerp_pixel( const image::pixel_t& a, const image::pixel_t& b, float t)
{
    float t_inv = 1.0f - t;
    return image::pixel_t(  ( get_color( a, red_t())   * t) + ( get_color( b, red_t())   * t_inv),
			    ( get_color( a, green_t()) * t) + ( get_color( b, green_t()) * t_inv),
			    ( get_color( a, blue_t())  * t) + ( get_color( b, blue_t())  * t_inv),
			    ( get_color( a, alpha_t()) * t) + ( get_color( b, alpha_t()) * t_inv));
}

} // detail

over_layer_mode_fun::over_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t over_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    float a = get_color( front, alpha_t()) * opacity_;
    float a_inv = 1.0f - a;

    return pixel_t( get_color( front, red_t()) * opacity_ + get_color( back, red_t())   * a_inv,
		    get_color( front, green_t()) * opacity_ + get_color( back, green_t()) * a_inv,
		    get_color( front, blue_t()) * opacity_  + get_color( back, blue_t())  * a_inv,
		    adobe::clamp( a + get_color( back, alpha_t()), 0.0f, 1.0f));
}

add_layer_mode_fun::add_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t add_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( get_color( front, red_t()) * opacity_ + get_color( back, red_t()),
		    get_color( front, green_t()) * opacity_ + get_color( back, green_t()),
		    get_color( front, blue_t()) * opacity_ + get_color( back, blue_t()),
		    adobe::clamp( get_color( front, alpha_t()) * opacity_ + get_color( back, alpha_t()), 0.0f, 1.0f));
}

mul_layer_mode_fun::mul_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t mul_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( ( get_color( front, red_t()) * opacity_ + 1.0f - opacity_)  * get_color( back, red_t()),
		    ( get_color( front, green_t()) * opacity_ + 1.0f - opacity_) * get_color( back, green_t()),
		    ( get_color( front, blue_t()) * opacity_ + 1.0f - opacity_)  * get_color( back, blue_t()),
		    ( get_color( front, alpha_t()) * opacity_ + 1.0f - opacity_) * get_color( back, alpha_t()));
}

sub_layer_mode_fun::sub_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t sub_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( get_color( back, red_t())   - get_color( front, red_t())   * opacity_,
		    get_color( back, green_t()) - get_color( front, green_t()) * opacity_,
		    get_color( back, blue_t())  - get_color( front, blue_t())  * opacity_,
		    adobe::clamp( get_color( back, alpha_t()) - get_color( front, alpha_t()) * opacity_, 0.0f, 1.0f));
}

mix_layer_mode_fun::mix_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t mix_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    float a = opacity_;
    float a_inv = 1.0f - a;

    return pixel_t( a * get_color( front, red_t())   + get_color( back, red_t())  * a_inv,
		    a * get_color( front, green_t()) + get_color( back, green_t()) * a_inv,
		    a * get_color( front, blue_t())  + get_color( back, blue_t()) * a_inv,
		    adobe::clamp( a * get_color( front, alpha_t()) + get_color( back, alpha_t()) * a_inv , 0.0f, 1.0f));
}

screen_layer_mode_fun::screen_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t screen_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( 1.0f - ((1.0f - get_color( front, red_t()))   * (1.0f - get_color( back, red_t()))),
		    1.0f - ((1.0f - get_color( front, green_t())) * (1.0f - get_color( back, green_t()))),
		    1.0f - ((1.0f - get_color( front, blue_t()))  * (1.0f - get_color( back, blue_t()))),
		    adobe::clamp( get_color( front, alpha_t()) + get_color( back, alpha_t()), 0.0f, 1.0f));

    get_color( result, red_t())     = std::max( get_color( result, red_t())  , get_color( back, red_t()));
    get_color( result, green_t())   = std::max( get_color( result, green_t()), get_color( back, green_t()));
    get_color( result, blue_t())    = std::max( get_color( result, blue_t()) , get_color( back, blue_t()));
    return detail::lerp_pixel( result, back, opacity_);
}

overlay_layer_mode_fun::overlay_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t overlay_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    float rb = get_color( back, red_t());
    float gb = get_color( back, green_t());
    float bb = get_color( back, blue_t());

    float rf = get_color( front, red_t());
    float gf = get_color( front, green_t());
    float bf = get_color( front, blue_t());

    rf = overlay( rf, rb);
    gf = overlay( gf, gb);
    bf = overlay( bf, bb);

    pixel_t result( rf, gf, bf, adobe::clamp( get_color( front, alpha_t()) + get_color( back, alpha_t()), 0.0f, 1.0f));
    return detail::lerp_pixel( result, back, opacity_);
}

zero_overlay_layer_mode_fun::zero_overlay_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t zero_overlay_layer_mode_fun::operator()( const pixel_t& p) const
{
    float rb = get_color( p, red_t());
    float gb = get_color( p, green_t());
    float bb = get_color( p, blue_t());

    float rf = overlay( 0, rb);
    float gf = overlay( 0, gb);
    float bf = overlay( 0, bb);

    pixel_t result( rf, gf, bf, get_color( p, alpha_t()));
    return detail::lerp_pixel( result, p, opacity_);
}

diff_layer_mode_fun::diff_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t diff_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( abs( get_color( back, red_t())   - get_color( front, red_t())),
		    abs( get_color( back, green_t()) - get_color( front, green_t())),
		    abs( get_color( back, blue_t())  - get_color( front, blue_t())),
		    abs( adobe::clamp( get_color( back, alpha_t()) - get_color( front, alpha_t()), 0.0f, 1.0f)));

    return detail::lerp_pixel( result, back, opacity_);
}

max_layer_mode_fun::max_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t max_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( std::max( get_color( front, red_t())   ,get_color( back, red_t())),
		    std::max( get_color( front, green_t()) ,get_color( back, green_t())),
		    std::max( get_color( front, blue_t())  ,get_color( back, blue_t())),
		    std::max( get_color( front, alpha_t()) ,get_color( back, alpha_t())));

    return detail::lerp_pixel( result, back, opacity_);
}

min_layer_mode_fun::min_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t min_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( std::min( get_color( front, red_t())   ,get_color( back, red_t())),
		    std::min( get_color( front, green_t()) ,get_color( back, green_t())),
		    std::min( get_color( front, blue_t())  ,get_color( back, blue_t())),
		    std::min( get_color( front, alpha_t()) ,get_color( back, alpha_t())));

    return detail::lerp_pixel( result, back, opacity_);
}

} // namespace
} // namespace
} // namespace

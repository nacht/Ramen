// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/reader.hpp>

#include<algorithm>

#include<boost/bind.hpp>

#include<half/halfExpLog.h>

namespace ramen
{
namespace imageio
{

namespace
{

struct apply_gamma_fun
{
    apply_gamma_fun( float f) : g_( f) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
        using namespace boost::gil;

	image::pixel_t result;
	half h;

	h = std::max( (float) get_color( src, red_t()), 0.0f);
	h = pow_h( h, g_);
	get_color( result, red_t()) = h;

	h = std::max( (float) get_color( src, green_t()), 0.0f);
	h = pow_h( h, g_);
	get_color( result, green_t()) = h;

	h = std::max( (float) get_color( src, blue_t()), 0.0f);
	h = pow_h( h, g_);
	get_color( result, blue_t()) = h;

	get_color( result, alpha_t()) = get_color( src, alpha_t());
	return result;
    }

private:

    float g_;
};

struct srgb_to_linear_fun
{
    srgb_to_linear_fun() {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
        using namespace boost::gil;

        return image::pixel_t(  srgb2linear( get_color( src, red_t())),
                                srgb2linear( get_color( src, green_t())),
                                srgb2linear( get_color( src, blue_t())),
                                get_color( src, alpha_t()));
    }

private:

    inline float srgb2linear( float x) const
    {
	const float k0 = 0.04045f;
	const float phi = 12.92f;
        const float alpha = 0.055;
        const float exponent = 2.4;

	if ( x <= k0 )
            return x / phi;

        return pow_h( ( std::max( x, 0.0f) + alpha) / ( 1.0f + alpha), exponent);
    }
};

} // unnamed

reader_t::reader_t( const boost::filesystem::path& p) : path_(p)
{
    info_[adobe::name_t( "gamma")]  = adobe::any_regular_t( 1.0);
}

const adobe::dictionary_t& reader_t::image_info() const { return info_;}

Imath::Box2i reader_t::domain() const
{
    return adobe::get_value( info_, adobe::name_t( "domain")).cast<Imath::Box2i>();
}

Imath::Box2i reader_t::defined() const
{
    Imath::Box2i defined( domain());
    adobe::get_value( info_, adobe::name_t( "defined"), defined);
    return defined;
}

void reader_t::read_image( const image::image_view_t& view) const
{
    read_image( view, defined(), 1);
}

void reader_t::read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    do_read_image( view, crop, subsample);

    bool sRGB = false;
    adobe::get_value( info_, adobe::name_t( "sRGB"), sRGB);

    if( sRGB)
        boost::gil::transform_pixels( view, view, srgb_to_linear_fun());
    else
    {
	float gamma = adobe::get_value( info_, adobe::name_t( "gamma")).cast<float>();

	if( gamma != 1.0f)
	    apply_gamma( view, view, gamma);
    }
}

void reader_t::repeat_scanline_until_end(const image::image_view_t& view, int y) const
{
    int last_scan = y;
    int yy = y + 1;

    while( yy < view.height())
    {
        std::copy( view.row_begin( last_scan), view.row_end( last_scan), view.row_begin( yy));
        ++yy;
    }
}

void reader_t::apply_gamma( const image::const_image_view_t& src, const image::image_view_t& dst, float gamma) const
{
    boost::gil::transform_pixels( src, dst, apply_gamma_fun( gamma));
}

} // namespace
} // namespace

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/hsv_colorize_node.hpp>

#include<algorithm>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/image/processing.hpp>

#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>

namespace ramen
{

namespace
{

float get_hue( const Imath::Color4f& c)
{
    float r = c.r;
    float g = c.g;
    float b = c.b;

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
    }

    return h;
}

struct hsv_colorize_fun
{
    hsv_colorize_fun( float hue, float sat, float val)
    {
        hue_ = hue;
        sat_ = sat;
        val_ = val;
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
        using namespace boost::gil;
        return image::pixel_t( hue_, get_color( p, green_t()) * sat_,
                                get_color( p, blue_t()) * val_,
                                get_color( p, alpha_t()));
    }

private:

    float hue_;
    float sat_;
    float val_;
};

struct clamp_blacks_fun
{
    image::pixel_t operator()( const image::pixel_t& p) const
    {
        using namespace boost::gil;
        return image::pixel_t(  std::max( (float) get_color( p, red_t())  , 0.0f),
                                std::max( (float) get_color( p, green_t()), 0.0f),
                                std::max( (float) get_color( p, blue_t()) , 0.0f),
                                get_color( p, alpha_t()));
    }
};

} // unnamed

hsv_colorize_node_t::hsv_colorize_node_t() : pointop_node_t() { set_name("hsv_colorize");}

void hsv_colorize_node_t::do_create_params()
{
    std::auto_ptr<color_param_t> q( new color_param_t( "Color"));
    q->set_id( "color");
    q->set_is_rgba( false);
    q->set_default_value( Imath::Color4f( 1, 0, 0, 1));
    add_param( q);

    std::auto_ptr<float_param_t> p( new float_param_t( "Strength"));
    p->set_id( "strength");
    p->set_range( 0, 1);
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);

    p.reset( new float_param_t( "Saturation"));
    p->set_id( "sat");
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);

    p.reset( new float_param_t( "Value"));
    p->set_id( "value");
    p->set_min( 0);
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);
}

void hsv_colorize_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    image::convert_rgb_to_hsv( src, dst);

    boost::gil::tbb_transform_pixels( dst, dst, hsv_colorize_fun( get_hue( get_value<Imath::Color4f>( param( "color"))),
                                                                  get_value<float>( param( "sat")),
                                                                  get_value<float>( param( "value"))));

    image::convert_hsv_to_rgb( dst, dst);
    image::lerp_images( dst, src, get_value<float>( param( "strength")), dst);
    boost::gil::tbb_transform_pixels( dst, dst, clamp_blacks_fun());
}

void hsv_colorize_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void hsv_colorize_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_hsv_colorize_node() { return new hsv_colorize_node_t();}

node_info_t hsv_colorize_node_info()
{
    node_info_t info;
    info.id = "builtin.hsv_colorize";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "HSV Colorize";
    info.create = &create_hsv_colorize_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( hsv_colorize_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::hsv_colorize_node_t)

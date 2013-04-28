// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/key/lum_key_node.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<IECore/ColorAlgo.h>

#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/image/color.hpp>
#include<ramen/image/channel_range.hpp>
#include<ramen/image/processing.hpp>

namespace ramen
{

namespace
{

struct lum_key_fun
{
    lum_key_fun( const Imath::Color4f& c, const Imath::V2f& tol, const Imath::V2f& soft)
    {
        range_.center = IECore::luminance( c);
        range_.tol_lo = tol.x;
        range_.tol_hi = tol.y;
        range_.soft_lo = soft.x;
        range_.soft_hi = soft.y;
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
        image::pixel_t result( src);
        float x = image::luminance( src);
        float a = 1.0f - range_( x);
        boost::gil::get_color( result, boost::gil::alpha_t()) = a;
        return result;
    }

private:

    image::channel_range_t range_;
};

struct remap_alpha_unpremult_fun
{
    remap_alpha_unpremult_fun( float lo, float hi) : alpha_low_( lo), alpha_high_( hi) {}

    image::pixel_t operator()( const image::pixel_t& p) const
    {
	float r = boost::gil::get_color( p, boost::gil::red_t());
	float g = boost::gil::get_color( p, boost::gil::green_t());
	float b = boost::gil::get_color( p, boost::gil::blue_t());
	float a = boost::gil::get_color( p, boost::gil::alpha_t());

	if( a <= alpha_low_)
	    a = 0;
	else
	{
	    if( a >= alpha_high_ || alpha_high_ == alpha_low_)
		a = 1;
	    else
		a = ( a - alpha_low_) / ( alpha_high_ - alpha_low_);
	}

	return image::pixel_t( r, g, b, a);
    }

private:

    float alpha_low_, alpha_high_;
};

} // unnamed

lum_key_node_t::lum_key_node_t() : pointop_node_t() { set_name("lum_key");}

void lum_key_node_t::do_create_params()
{
    std::auto_ptr<color_param_t> c( new color_param_t( "Luminance"));
    c->set_id( "luminance");
    c->set_is_rgba( false);
    c->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( c);

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Tolerance"));
    p->set_id( "tolerance");
    p->set_default_value( Imath::V2f( 0.1, 0.1));
    p->set_min( 0);
    p->set_step( 0.05f);
    p->set_proportional( true);
    add_param( p);

    p.reset( new float2_param_t( "Softness"));
    p->set_id( "softness");
    p->set_default_value( Imath::V2f( 0, 0));
    p->set_min( 0);
    p->set_step( 0.05f);
    p->set_proportional( true);
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Alpha Low"));
    q->set_id( "alpha_low");
    q->set_default_value( 0);
    q->set_range( 0, 1);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Alpha High"));
    q->set_id( "alpha_high");
    q->set_default_value( 1);
    q->set_range( 0, 1);
    q->set_step( 0.05);
    add_param( q);

    std::auto_ptr<bool_param_t> b( new bool_param_t( "Invert"));
    b->set_id( "invert");
    b->set_default_value( false);
    add_param( b);

    b.reset( new bool_param_t( "Premultiply"));
    b->set_id( "premult");
    b->set_default_value( false);
    add_param( b);
}

void lum_key_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, lum_key_fun( get_value<Imath::Color4f>( param( "luminance")),
                                                             get_value<Imath::V2f>( param( "tolerance")),
                                                             get_value<Imath::V2f>( param( "softness"))));

    boost::gil::tbb_transform_pixels( dst, dst, remap_alpha_unpremult_fun( get_value<float>( param( "alpha_low")),
                                                                            get_value<float>( param( "alpha_high"))));

    if( get_value<bool>( param( "invert")))
	image::invert_alpha( dst, dst);

    if( get_value<bool>( param( "premult")))
	image::premultiply( dst, dst);
}

void lum_key_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void lum_key_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_lum_key_node() { return new lum_key_node_t();}

node_info_t lum_key_node_info()
{
    node_info_t info;
    info.id = "builtin.lum_key";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Key";
    info.menu_item = "Luminance Key";
    info.create = &create_lum_key_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( lum_key_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::lum_key_node_t)

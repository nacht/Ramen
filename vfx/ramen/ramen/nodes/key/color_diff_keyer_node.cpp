// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/key/color_diff_keyer_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

namespace ramen
{

namespace
{

struct color_diff_key_fun
{
public:

    color_diff_key_fun( const Imath::Color4f& key, float screen, float complement, float shadows, float alpha_low, float alpha_high, int output)
    {
	key_ = key;
	screen_ = screen;
	complement_ = complement;
	shadows_ = shadows;
	alpha_low_ = alpha_low;
	alpha_high_ = alpha_high;
	output_ = output;
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
	static image::pixel_t one( 1, 1, 1, 1);
	return (*this)( p, one);
    }

    image::pixel_t operator()( const image::pixel_t& p, const image::pixel_t& msk) const
    {
	float rix, gix, bix, rsc, gsc, bsc;

	if( key_.g > key_.b)
	{
	    rix = boost::gil::get_color( p, boost::gil::red_t());
	    gix = boost::gil::get_color( p, boost::gil::green_t());
	    bix = boost::gil::get_color( p, boost::gil::blue_t());
	    rsc = key_.r;
	    gsc = key_.g;
	    bsc = key_.b;
	}
	else
	{
	    rix = boost::gil::get_color( p, boost::gil::red_t());
	    gix = boost::gil::get_color( p, boost::gil::blue_t());
	    bix = boost::gil::get_color( p, boost::gil::green_t());
	    rsc = key_.r;
	    gsc = key_.b;
	    bsc = key_.g;
	}

	float ax = 1.0f - ( screen_ * adobe::clamp( gix, 0.0f, 1.0f));
	ax += ( complement_ * std::max( adobe::clamp( rix - rsc, 0.0f, 1.0f), adobe::clamp( bix - bsc, 0.0f, 1.0f)));

	if( gsc != 0 && gix < gsc)
	    ax += shadows_ * ( 1.0f - gix / gsc);

	if( ax <= alpha_low_)
	    ax = 0;
	else
	{
	    if( ax >= alpha_high_ || alpha_high_ == alpha_low_)
		ax = 1;
	    else
		ax = ( ax - alpha_low_) / ( alpha_high_ - alpha_low_);
	}

	ax *= boost::gil::get_color( msk, boost::gil::alpha_t());

	image::pixel_t result;

	switch( output_)
	{
	case 0:
	    boost::gil::get_color( result, boost::gil::red_t())   = boost::gil::get_color( p, boost::gil::red_t()) * ax;
	    boost::gil::get_color( result, boost::gil::green_t()) = boost::gil::get_color( p, boost::gil::green_t()) * ax;
	    boost::gil::get_color( result, boost::gil::blue_t())  = boost::gil::get_color( p, boost::gil::blue_t()) * ax;
	    boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
	break;

	case 1:
	    boost::gil::get_color( result, boost::gil::red_t())   = boost::gil::get_color( p, boost::gil::red_t());
	    boost::gil::get_color( result, boost::gil::green_t()) = boost::gil::get_color( p, boost::gil::green_t());
	    boost::gil::get_color( result, boost::gil::blue_t())  = boost::gil::get_color( p, boost::gil::blue_t());
	    boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
	break;

	case 2: // alpha
	    boost::gil::get_color( result, boost::gil::red_t())   = ax;
	    boost::gil::get_color( result, boost::gil::green_t()) = ax;
	    boost::gil::get_color( result, boost::gil::blue_t())  = ax;
	    boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
	break;

	case 3: // status
	    if( ax > 0.0f && ax < 1.0f)
		ax = 0.5f;

	    boost::gil::get_color( result, boost::gil::red_t())   = ax;
	    boost::gil::get_color( result, boost::gil::green_t()) = ax;
	    boost::gil::get_color( result, boost::gil::blue_t())  = ax;
	    boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
	break;
	}

	return result;
    }

private:

    Imath::Color4f key_;
    float screen_;
    float complement_;
    float shadows_;
    float alpha_low_;
    float alpha_high_;
    int output_;
};

} // unnamed

color_diff_keyer_node_t::color_diff_keyer_node_t() : image_node_t()
{
    set_name("col_diff_key");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["matte plug"], "Mask"), true);
    add_output_plug();
}

void color_diff_keyer_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Output"));
    r->set_id( "output");
    r->menu_items() += "Premultiplied", "Unpremultiplied", "Alpha", "Status";
    add_param( r);

    std::auto_ptr<color_param_t> p( new color_param_t( "Color"));
    p->set_id( "color");
    p->set_is_rgba( false);
    p->set_default_value( Imath::Color4f( 0, 0, 1, 0));
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Screen"));
    q->set_id( "screen_mult");
    q->set_default_value( 1);
    q->set_range( 0, 10);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Complement"));
    q->set_id( "comp_mult");
    q->set_default_value( 1);
    q->set_range( 0, 10);
    q->set_step( 0.05);
    add_param( q);
    
    q.reset( new float_param_t( "Shadows"));
    q->set_id( "shd_mult");
    q->set_default_value( 0.0);
    q->set_range( 0, 10);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Alpha Low"));
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
}

void color_diff_keyer_node_t::do_calc_bounds( const render::render_context_t& context)
{
    if( input( 1))
	set_bounds( intersect( input(0)->bounds(), input(1)->bounds()));
    else
	set_bounds( input(0)->bounds());
}

void color_diff_keyer_node_t::do_calc_defined( const render::render_context_t& context)
{
    if( input( 1))
    {
	Imath::Box2i def( intersect( input(0)->defined(), input(1)->defined()));
	set_defined( def);
    }
    else
	set_defined( input(0)->defined());
}

void color_diff_keyer_node_t::do_process( const render::render_context_t& context)
{
    if( !input(1))
    {
	Imath::Box2i area( intersect( defined(), input(0)->defined()));

	if( area.isEmpty())
	    return;

	boost::gil::tbb_transform_pixels(  input(0)->const_subimage_view( area),
						subimage_view( area),
						color_diff_key_fun( get_value<Imath::Color4f>( param( "color")),
						get_value<float>( param( "screen_mult")),
						get_value<float>( param( "comp_mult")),
						get_value<float>( param( "shd_mult")),
						get_value<float>( param( "alpha_low")),
						get_value<float>( param( "alpha_high")),
						get_value<int>( param( "output"))));
    }
    else
    {
	Imath::Box2i area( intersect( intersect( defined(), input(0)->defined()), input(1)->defined()));

	if( area.isEmpty())
	    return;

	boost::gil::tbb_transform_pixels(  input(0)->const_subimage_view( area),
						input(1)->const_subimage_view( area),
						subimage_view( area),
						color_diff_key_fun( get_value<Imath::Color4f>( param( "color")),
						get_value<float>( param( "screen_mult")),
						get_value<float>( param( "comp_mult")),
						get_value<float>( param( "shd_mult")),
						get_value<float>( param( "alpha_low")),
						get_value<float>( param( "alpha_high")),
						get_value<int>( param( "output"))));
    }
}

void color_diff_keyer_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void color_diff_keyer_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

// factory

node_t *create_color_diff_keyer_node() { return new color_diff_keyer_node_t();}

node_info_t color_diff_keyer_node_info()
{
    node_info_t info;
    info.id = "builtin.color_diff_keyer";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Key";
    info.menu_item = "Color Diff Keyer";
    info.create = &create_color_diff_keyer_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( color_diff_keyer_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::color_diff_keyer_node_t)

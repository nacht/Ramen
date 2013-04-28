// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/tonemap/global_tonemap_node.hpp>

#include<algorithm>

#include<adobe/algorithm/clamp.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/color_param.hpp>

namespace ramen
{

namespace
{

enum
{
    exr_tonemap_method      = 0,
    rational_tonemap_method = 1
};

struct exr_tonemap_fun
{
    typedef image::pixel_t result_type;

    exr_tonemap_fun( float exposure, const Imath::Color4f& fog, float defog, float knee_low, float knee_high)
    {
	exposure_ = std::pow( 2.0, exposure + 2.47393);
	fog_ = fog;
	defog_ = defog;
	knee_low_ = std::pow( 2.0, (double) knee_low);
	f_ = find_knee_f( std::pow( 2.0, (double) knee_high) - knee_low_, std::pow( 2.0, 3.5) - knee_low_);
	scale_ = std::pow( 2.0, -3.5);
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
	return image::pixel_t(	apply( boost::gil::get_color( p, boost::gil::red_t())  , fog_.r * defog_),
				apply( boost::gil::get_color( p, boost::gil::green_t()), fog_.g * defog_),
				apply( boost::gil::get_color( p, boost::gil::blue_t()) , fog_.b * defog_),
				boost::gil::get_color( p, boost::gil::alpha_t()));
    }

    float apply( float x, float fog) const
    {
	float r = std::max( x - fog, 0.0f);
	r *= exposure_;

	if( r < knee_low_)
	    r = knee_low_ + knee( r - knee_low_, f_);

	return r * scale_;
    }

    float find_knee_f( float x, float y) const
    {
	float f0 = 0;
	float f1 = 1;

	while( knee( x, f1) > y)
	{
	    f0 = f1;
	    f1 = f1 * 2;
	}

	for (int i = 0; i < 30; ++i)
	{
	    float f2 = (f0 + f1) / 2;
	    float y2 = knee (x, f2);

	    if (y2 < y)
		f1 = f2;
	    else
		f0 = f2;
	}

	return (f0 + f1) / 2;
    }

    float knee( double x, double f) const
    {
	return float( std::log( x * f + 1) / f);
    }

    float exposure_, defog_, knee_low_, f_, scale_;
    Imath::Color4f fog_;
};

struct rational_tonemap_fun
{
    rational_tonemap_fun( const Imath::Color4f& high, float p) : p_(p)
    {
        high_val_ = ( high.r * 0.212671f) + ( high.g * 0.715160f) + ( high.b * 0.072169f);
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
	return image::pixel_t(	apply( boost::gil::get_color( p, boost::gil::red_t())),
				apply( boost::gil::get_color( p, boost::gil::green_t())),
				apply( boost::gil::get_color( p, boost::gil::blue_t())),
				boost::gil::get_color( p, boost::gil::alpha_t()));
    }

private:

    float apply( float x) const { return ( x * p_) / (( x * p_) - x + high_val_);}

    float high_val_;
    float p_;
};

} // unnamed

global_tonemap_node_t::global_tonemap_node_t() : pointop_node_t() { set_name( "global_tmap");}

void global_tonemap_node_t::do_create_params()
{
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Method"));
    top->set_id( "method");

    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "EXR"));
	group->set_id( "exr");

        std::auto_ptr<float_param_t> p( new float_param_t( "Exposure"));
        p->set_id( "exr_exposure");
        p->set_range( -10.0f, 10.0f);
        p->set_step( 0.25f);
        p->set_default_value( 0.0f);
	group->add_param( p);

        std::auto_ptr<color_param_t> q( new color_param_t( "Fog"));
        q->set_id( "exr_fog");
        q->set_is_rgba( false);
        group->add_param( q);

        p.reset( new float_param_t( "Defog"));
        p->set_id( "exr_defog");
        p->set_range( 0.0f, 0.01f);
        p->set_step( 0.001f);
        p->set_default_value( 0.0f);
        group->add_param( p);

        p.reset( new float_param_t( "Knee Low"));
        p->set_id( "exr_knee_low");
        p->set_range( -3.0f, 3.0f);
        p->set_step( 0.1f);
        p->set_default_value( 0.0f);
        group->add_param( p);

        p.reset( new float_param_t( "Knee High"));
        p->set_id( "exr_knee_high");
        p->set_range( 3.5f, 7.5f);
        p->set_step( 0.1f);
        p->set_default_value( 5.0f);
        group->add_param( p);
        
        top->add_param( group);
    }

    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Rational"));
	group->set_id( "rational");

        std::auto_ptr<color_param_t> q( new color_param_t( "High Value"));
        q->set_id( "rat_high");
        q->set_is_rgba( false);
        q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
        group->add_param( q);

        std::auto_ptr<float_param_t> p( new float_param_t( "Response"));
        p->set_id( "rat_response");
        p->set_min( 1.0f);
        p->set_step( 0.1f);
        p->set_default_value( 1.0f);
	group->add_param( p);

        top->add_param( group);
    }

    add_param( top);
}

void global_tonemap_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    switch( get_value<int>( param( "method")))
    {
    case exr_tonemap_method:
        boost::gil::tbb_transform_pixels( src, dst, exr_tonemap_fun( get_value<float>( param( "exr_exposure")),
                                                                         get_value<Imath::Color4f>( param( "exr_fog")),
                                                                         get_value<float>( param( "exr_defog")),
                                                                         get_value<float>( param( "exr_knee_low")),
                                                                         get_value<float>( param( "exr_knee_high"))));
    break;

    case rational_tonemap_method:
        boost::gil::tbb_transform_pixels( src, dst, rational_tonemap_fun( get_value<Imath::Color4f>( param( "rat_high")),
                                                                          get_value<float>( param( "rat_response"))));
    break;
    }
}

void global_tonemap_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void global_tonemap_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_global_tonemap_node() { return new global_tonemap_node_t();}

node_info_t global_tonemap_node_info()
{
    node_info_t info;
    info.id = "builtin.global_tonemap";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Tone Map";
    info.menu_item = "Global Tonemap";
    info.create = &create_global_tonemap_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( global_tonemap_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::global_tonemap_node_t)

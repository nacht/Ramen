// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/curves_node.hpp>

#include<boost/foreach.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<OpenEXR/halfLimits.h>

#include<ramen/params/curve_param.hpp>

#include<ramen/anim/curve_to_half_lut.hpp>

using namespace boost::gil;

namespace ramen
{

struct apply_curves_fun
{
    apply_curves_fun( const half_lut_t& r_lut, const half_lut_t& g_lut,
			const half_lut_t& b_lut, const half_lut_t& v_lut) : r_lut_( r_lut), g_lut_( g_lut),
										b_lut_( b_lut), v_lut_( v_lut)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	image::pixel_t dst;
	get_color( dst, red_t())    = r_lut_( v_lut_( get_color( src, red_t())));
	get_color( dst, green_t())  = g_lut_( v_lut_( get_color( src, green_t())));
	get_color( dst, blue_t())   = b_lut_( v_lut_( get_color( src, blue_t())));
	get_color( dst, alpha_t())  = get_color( src, alpha_t());
	return dst;
    }

private:
    
    const half_lut_t& r_lut_;
    const half_lut_t& g_lut_;
    const half_lut_t& b_lut_;
    const half_lut_t& v_lut_;
};

curves_node_t::curves_node_t() : pointop_node_t() { set_name("curves");}

void curves_node_t::add_curve_param( const std::string& name, const std::string& id, const Imath::Color3c col)
{
    std::auto_ptr<curve_param_t> p( new curve_param_t( name));
    p->set_id( id);
    p->set_min( 0.0f);
    p->set_max( std::numeric_limits<half>::max());
    p->set_color( col);

    p->curve().set_default_auto_tangents( anim::tangent_linear);
    p->curve().insert( 0, 0, false);
    p->curve().insert( 1, 1, false);
    p->curve().set_extrapolation( anim::extrapolate_linear);
    p->curve().recalc_tangents_and_coefficients();
    add_param( p);
}

void curves_node_t::do_create_params()
{
    add_curve_param( "RGB", "rgb");
    add_curve_param( "Red", "red", Imath::Color3c( 255, 0, 0));
    add_curve_param( "Green", "green", Imath::Color3c( 0, 255, 0));
    add_curve_param( "Blue", "blue", Imath::Color3c( 0, 0, 255));
}

void curves_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    half_lut_t r_lut, g_lut, b_lut, v_lut;

    const curve_param_t *c = dynamic_cast<const curve_param_t*>( &param( "rgb"));
    anim::curve_to_half_lut( c->curve(), v_lut);

    c = dynamic_cast<const curve_param_t*>( &param( "red"));
    anim::curve_to_half_lut( c->curve(), r_lut);

    c = dynamic_cast<const curve_param_t*>( &param( "green"));
    anim::curve_to_half_lut( c->curve(), g_lut);

    c = dynamic_cast<const curve_param_t*>( &param( "blue"));
    anim::curve_to_half_lut( c->curve(), b_lut);

    tbb_transform_pixels( src, dst, apply_curves_fun( r_lut, g_lut, b_lut, v_lut));
}

void curves_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void curves_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_curves_node() { return new curves_node_t();}

node_info_t curves_node_info()
{
    node_info_t info;
    info.id = "builtin.curves";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Curves";
    info.create = &create_curves_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( curves_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::curves_node_t)

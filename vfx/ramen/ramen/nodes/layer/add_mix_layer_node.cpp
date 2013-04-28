// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/layer/add_mix_layer_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/curve_param.hpp>

#include<ramen/image/color.hpp>

#include<ramen/anim/curve_to_half_lut.hpp>

namespace ramen
{

namespace
{

struct add_mix_layer_mode_unpremult_fun
{
    add_mix_layer_mode_unpremult_fun( const half_lut_t& fg_lut, const half_lut_t& bg_lut) : fg_lut_( fg_lut), bg_lut_( bg_lut)
    {
    }

    image::pixel_t operator()( const image::pixel_t& back, const image::pixel_t& front) const
    {
	using namespace boost::gil;
	
	float a	    = fg_lut_( get_color( front, alpha_t()));
	float a_inv = bg_lut_( get_color( front, alpha_t()));

	return image::pixel_t(  get_color( front, red_t())   * a + get_color( back, red_t())   * a_inv,
				get_color( front, green_t()) * a + get_color( back, green_t()) * a_inv,
				get_color( front, blue_t())  * a + get_color( back, blue_t())  * a_inv,
				adobe::clamp( a + get_color( back, alpha_t()), 0.0f, 1.0f));
    }

private:

    const half_lut_t& fg_lut_;
    const half_lut_t& bg_lut_;
};

struct add_mix_layer_mode_premult_fun
{
    add_mix_layer_mode_premult_fun( const half_lut_t& fg_lut, const half_lut_t& bg_lut) : fun_( fg_lut, bg_lut) {}

    image::pixel_t operator()( const image::pixel_t& back, const image::pixel_t& front) const
    {
        return fun_( back, image::unpremultiply_pixel( front));
    }

private:

    add_mix_layer_mode_unpremult_fun fun_;
};

}

add_mix_layer_node_t::add_mix_layer_node_t() : base_layer_node_t()
{
    set_name( "add_mix");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["back plug"], "Back"));
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void add_mix_layer_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->menu_items() += "Background", "Foreground";
    add_param( p);

    std::auto_ptr<bool_param_t> b( new bool_param_t( "Premultiplied"));
    b->set_id( "premult");
    b->set_default_value( true);
    add_param( b);

    std::auto_ptr<curve_param_t> c( new curve_param_t( "Fg"));
    c->set_id( "fg");
    c->set_min( 0.0f);
    c->set_max( 1.0f);
    c->set_color( Imath::Color3c( 255, 0, 0));

    c->curve().set_default_auto_tangents( anim::tangent_linear);
    c->curve().insert( 0, 0, false);
    c->curve().insert( 1, 1, false);
    c->curve().recalc_tangents_and_coefficients();
    add_param( c);

    c.reset( new curve_param_t( "Bg"));
    c->set_id( "bg");
    c->set_min( 0.0f);
    c->set_max( 1.0f);
    c->set_color( Imath::Color3c( 0, 255, 0));

    c->curve().set_default_auto_tangents( anim::tangent_linear);
    c->curve().insert( 0, 1, false);
    c->curve().insert( 1, 0, false);
    c->curve().recalc_tangents_and_coefficients();
    add_param( c);
}

void add_mix_layer_node_t::do_calc_domain( const render::render_context_t& context)
{
    if( get_value<int>( param( "mode")))
	set_domain( input(1)->domain());
    else
	set_domain( input(0)->domain());
}

void add_mix_layer_node_t::do_calc_bounds( const render::render_context_t& context)
{
    Imath::Box2i bbox( input(0)->bounds());
    bbox.extendBy( input(1)->bounds());
    set_bounds( bbox);
}

void add_mix_layer_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i bbox( input(0)->defined());
    bbox.extendBy( input(1)->defined());
    set_defined( bbox);
}

void add_mix_layer_node_t::do_process( const render::render_context_t& context)
{
    node_t *bg = input(0);
    node_t *fg = input(1);

    Imath::Box2i bg_area = intersect( bg->defined(), defined());

    if( !bg_area.isEmpty())
    {
        render_input( 0, context);
	boost::gil::copy_pixels( bg->const_subimage_view( bg_area), subimage_view( bg_area));
        release_input_image( 0);
    }

    Imath::Box2i comp_area( intersect( fg->defined(), defined()));

    if( !comp_area.isEmpty())
    {
        render_input( 1, context);

	half_lut_t fg_lut, bg_lut;

	const curve_param_t *c = dynamic_cast<const curve_param_t*>( &param( "fg"));
        anim::curve_to_half_lut( c->curve(), fg_lut);

	c = dynamic_cast<const curve_param_t*>( &param( "bg"));
        anim::curve_to_half_lut( c->curve(), bg_lut);

	if( get_value<bool>( param( "premult")))
	{
	    boost::gil::tbb_transform_pixels( const_subimage_view( comp_area),
						input(1)->const_subimage_view( comp_area),
						subimage_view( comp_area), add_mix_layer_mode_premult_fun( fg_lut, bg_lut));
	}
	else
	{
	    boost::gil::tbb_transform_pixels( const_subimage_view( comp_area),
						input(1)->const_subimage_view( comp_area),
						subimage_view( comp_area), add_mix_layer_mode_unpremult_fun( fg_lut, bg_lut));
	}

        release_input_image( 1);
    }
}

void add_mix_layer_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( base_layer_node_t);
}

void add_mix_layer_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( base_layer_node_t);
}

// factory

base_layer_node_t *create_add_mix_layer_node() { return new add_mix_layer_node_t();}

node_info_t add_mix_layer_node_info()
{
    node_info_t info;
    info.id = "builtin.addmix.layer";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Layer";
    info.menu_item = "AddMix";
    info.create = &create_add_mix_layer_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node(add_mix_layer_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::add_mix_layer_node_t)

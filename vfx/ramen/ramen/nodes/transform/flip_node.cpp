// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/transform/flip_node.hpp>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

flip_node_t::flip_node_t() : image_node_t()
{
    set_name("flip");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void flip_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->menu_items() += "Horizontal", "Vertical", "Both";
    add_param( p);
}

void flip_node_t::calc_transform_matrix( int subsample)
{
    float cx = input()->domain().center().x;
    float cy = input()->domain().center().y;

    float sx, sy;

    switch( get_value<int>( param( "mode")))
    {
    case 0:
	sx = -1.0f;
	sy =  1.0f;
    break;

    case 1:
	sx =  1.0f;
	sy = -1.0f;
    break;

    case 2:
	sx = -1.0f;
	sy = -1.0f;
    break;
    }

    xform_ = Imath::M33d().setTranslation( Imath::V2d( -cx, -cy)) *
             Imath::M33d().setScale( Imath::V2d( sx, sy)) *
             Imath::M33d().setTranslation( Imath::V2d( cx, cy));

    inv_xform_ = xform_.inverse();
}

void flip_node_t::do_calc_bounds( const render::render_context_t& context)
{
    calc_transform_matrix();
    Imath::Box2i xfbounds( input()->bounds());
    xfbounds = transform( xfbounds, xform_);
    set_bounds( xfbounds);
}

void flip_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    Imath::Box2i roi( transform( interest(), inv_xform_));
    input()->add_interest( roi);
}

void flip_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( intersect( bounds(), interest()));
}

void flip_node_t::do_process( const render::render_context_t& context)
{
    if( defined().isEmpty())
        return;

    calc_transform_matrix( context.subsample);
    image::affine_warp_nearest( input()->defined(), input()->const_image_view(),
					defined(), image_view(), xform_, inv_xform_);
}

void flip_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

void flip_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

// factory
image_node_t *create_flip_node() { return new flip_node_t();}

node_info_t flip_node_info()
{
    node_info_t info;
    info.id = "builtin.flip";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Flip";
    info.create = &create_flip_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node(flip_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::flip_node_t)

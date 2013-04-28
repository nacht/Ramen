// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/util/set_dod_node.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{

set_dod_node_t::set_dod_node_t() : image_node_t()
{
    set_name( "dod");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void set_dod_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Top"));
    p->set_id( "top");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Left"));
    p->set_id( "left");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Bottom"));
    p->set_id( "bottom");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Right"));
    p->set_id( "right");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);
}

void set_dod_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    Imath::Box2i box( domain());
    int crop_t = get_absolute_value<float>( param( "top"));
    int crop_l = get_absolute_value<float>( param( "left"));
    int crop_b = get_absolute_value<float>( param( "bottom"));
    int crop_r = get_absolute_value<float>( param( "right"));
    Imath::Box2i def( Imath::V2i( box.min.x + crop_l, box.min.y + crop_t), Imath::V2i( box.max.x - crop_r, box.max.y - crop_b));
    Imath::Box2i roi = intersect( interest(), def);
    input()->add_interest( roi);
}

void set_dod_node_t::do_calc_bounds( const render::render_context_t& context)
{
    Imath::Box2i box( input()->domain());
    int crop_t = get_absolute_value<float>( param( "top"));
    int crop_l = get_absolute_value<float>( param( "left"));
    int crop_b = get_absolute_value<float>( param( "bottom"));
    int crop_r = get_absolute_value<float>( param( "right"));
    Imath::Box2i bounds( Imath::V2i( box.min.x + crop_l, box.min.y + crop_t), Imath::V2i( box.max.x - crop_r, box.max.y - crop_b));
    set_bounds( bounds);
}

void set_dod_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( bounds());
}

void set_dod_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area = intersect( input()->defined(), defined());

    if( area.isEmpty())
	return;

    boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
}

void set_dod_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

void set_dod_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

// factory
image_node_t *create_set_dod_node() { return new set_dod_node_t();}

node_info_t set_dod_node_info()
{
    node_info_t info;
    info.id = "builtin.setdod";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Util";
    info.menu_item = "Set DoD";
    info.create = &create_set_dod_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node(set_dod_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::set_dod_node_t)
